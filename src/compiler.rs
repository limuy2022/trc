pub mod ast;
pub mod linker;
pub mod llvm_convent;
pub mod manager;
pub mod optimizer;
pub mod scope;
pub mod token;

pub use self::{ast::ModuleUnit, manager::ModuleManager, token::TokenLex};
use crate::cfg;
use core::panic;
use libcore::*;
use rust_i18n::t;
use std::{
    cell::RefCell,
    collections::HashMap,
    fs,
    io::{self, BufRead},
    path::PathBuf,
    process::exit,
    rc::Rc,
    vec,
};

#[derive(Debug, Clone)]
/// 不同的输入源
pub enum InputSource {
    File(PathBuf),
    StringInternal,
}

impl InputSource {
    /// 获取文件路径，非文件直接返回main
    pub fn get_path(&self) -> &str {
        match *self {
            InputSource::File(ref filename) => filename.to_str().unwrap(),
            _ => cfg::MAIN_MODULE_NAME,
        }
    }

    pub fn set_path(&mut self, name: PathBuf) {
        if let InputSource::File(filename) = self {
            *filename = name;
        } else {
            panic!("not a file error")
        }
    }
}

/// 编译器的参数控制
pub struct CompileOption {
    pub optimize: bool,
    pub inputsource: InputSource,
    pub file_save: PathBuf,
}

#[derive(Debug, Clone)]
pub struct Context {
    module_name: String,
    line: usize,
}

impl ErrorContext for Context {
    fn get_module_name(&self) -> &str {
        &self.module_name
    }

    fn get_line(&self) -> usize {
        self.line
    }
}

impl Context {
    pub fn new(module_name: &str) -> Self {
        Self {
            module_name: String::from(module_name),
            line: 0,
        }
    }

    pub fn new_line(module_name: &str, line: usize) -> Self {
        Self {
            module_name: String::from(module_name),
            line,
        }
    }

    pub fn add_line(&mut self) {
        self.line += 1;
    }

    pub fn del_line(&mut self) {
        self.line -= 1;
    }

    pub fn set_line(&mut self, line: usize) {
        self.line = line;
    }
}

impl CompileOption {
    pub fn new(optimize: bool, source: InputSource) -> Self {
        CompileOption {
            optimize,
            inputsource: source,
            file_save: PathBuf::from("."),
        }
    }
}

type Pool<T> = HashMap<T, usize>;

#[derive(Default)]
/// 管理常量池添加删除
pub struct ValuePool {
    const_strings: Pool<String>,
    const_floats: Pool<String>,
    name_pool: Pool<String>,
    _const_big_int: Pool<String>,
    pub id_float: Vec<f64>,
    pub id_str: Vec<String>,
    pub id_name: Vec<String>,
}

pub const INT_VAL_POOL_ZERO: usize = 0;
pub const INT_VAL_POOL_ONE: usize = 1;

macro_rules! gen_single_getter_setter {
    ($func_name:ident => ($const_pool:ident, $id_pool:ident, $type:ty)) => {
        paste::paste! {
        pub fn [<add_ $func_name>](&mut self, val: $type) -> usize {
            let len_tmp = self.$const_pool.len();
            let ret = *self.$const_pool.entry(val.clone()).or_insert(len_tmp);
            if len_tmp != self.$const_pool.len() {
                self.$id_pool.push(val);
            }
            ret
        }
        pub fn [<get_ $func_name>](&self, val: &$type) -> Option<usize> {
            self.$const_pool.get(val).copied()
        }
        }
    };
    ($func_name:ident => ($const_pool:ident, $id_pool:ident, $type:ty, $convent_func: ident)) => {
        paste::paste! {
        pub fn [<add_ $func_name>](&mut self, val: $type) -> usize {
            let len_tmp = self.$const_pool.len();
            let ret = *self.$const_pool.entry(val.clone()).or_insert(len_tmp);
            if len_tmp != self.$const_pool.len() {
                self.$id_pool.push($convent_func(val));
            }
            ret
        }
        pub fn [<get_ $func_name>](&self, val: &$type) -> Option<usize> {
            self.$const_pool.get(val).copied()
        }
        }
    };
}

macro_rules! gen_getter_setter {
    ($($func_name:ident => ($const_pool:ident, $id_pool:ident, $type:ty)),*) => {
        $(
        gen_single_getter_setter!($func_name => ($const_pool, $id_pool, $type));
        )*
    };
}

fn convert_str_to_float(s: String) -> f64 {
    s.parse::<f64>().unwrap()
}

impl ValuePool {
    fn new() -> Self {
        let ret = Self {
            ..Default::default()
        };
        ret
    }

    gen_getter_setter!(
        string => (const_strings, id_str, String),
        id => (name_pool, id_name, String)
    );
    gen_single_getter_setter!(float => (const_floats, id_float, String, convert_str_to_float));

    fn store_val_to_vm(&mut self) -> ConstPool {
        let mut ret = ConstPool::new();
        ret.floatpool.clone_from(&self.id_float);
        ret.stringpool.clone_from(&self.id_str);
        ret
    }

    pub fn extend_pool(&mut self, data: &StaticData) {
        for i in &data.constpool.floatpool {
            self.add_float((i).to_string());
        }
        for i in &data.constpool.stringpool {
            // TODO::improve copy performance
            self.add_string(i.clone());
        }
    }
}

#[derive(Debug)]
/// # Reference
/// reference iterator:<https://stackoverflow.com/questions/43952104/how-can-i-store-a-chars-iterator-in-the-same-struct-as-the-string-it-is-iteratin>
pub struct StringSource {
    text: String,
    pos: usize,
    prev_size: usize,
}

impl StringSource {
    pub fn new(source: impl Into<String>) -> Self {
        Self {
            text: source.into(),
            pos: 0,
            prev_size: 0,
        }
    }

    pub fn reset_source(&mut self, source: impl Into<String>) {
        self.pos = 0;
        self.prev_size = 0;
        self.text = source.into();
    }
}

impl Iterator for StringSource {
    type Item = char;

    fn next(&mut self) -> Option<Self::Item> {
        match self.read() {
            '\0' => None,
            other => Some(other),
        }
    }
}

impl TokenIo for StringSource {
    fn unread(&mut self, c: char) {
        if c == '\0' {
            return;
        }
        self.pos -= self.prev_size;
        // check if match the right char
        debug_assert_eq!(self.text[self.pos..].chars().next().unwrap(), c);
    }

    fn read(&mut self) -> char {
        let mut opt = self.text[self.pos..].chars();
        match opt.next() {
            None => '\0',
            Some(c) => {
                let sz = c.len_utf8();
                self.prev_size = sz;
                self.pos += sz;
                c
            }
        }
    }
}

pub trait TokenIo: Iterator {
    fn unread(&mut self, c: char);

    fn read(&mut self) -> char;
}

pub struct FileSource {
    back: Vec<char>,
    buf: io::BufReader<fs::File>,
    s: String,
    input_pos: usize,
}

impl FileSource {
    pub fn new(f: fs::File) -> Self {
        let buf = io::BufReader::new(f);
        let s = String::new();
        let mut ret = FileSource {
            back: vec![],
            buf,
            input_pos: 0,
            s,
        };
        ret.init_new_line();
        ret
    }

    fn init_new_line(&mut self) {
        self.s.clear();
        self.buf.read_line(&mut self.s).unwrap();
        self.input_pos = 0;
    }
}

impl Iterator for FileSource {
    type Item = char;
    fn next(&mut self) -> Option<Self::Item> {
        match self.read() {
            '\0' => None,
            other => Some(other),
        }
    }
}

impl TokenIo for FileSource {
    fn unread(&mut self, c: char) {
        self.back.push(c);
    }

    fn read(&mut self) -> char {
        if !self.back.is_empty() {
            return self.back.pop().unwrap();
        }
        loop {
            let mut input_pos = self.s[self.input_pos..].chars();
            match input_pos.next() {
                None => {
                    self.init_new_line();
                    if self.s.is_empty() {
                        return '\0';
                    }
                }
                Some(c) => {
                    self.input_pos += c.len_utf8();
                    return c;
                }
            }
        }
    }
}

pub struct CompilerImpl {
    // to support read from stdin and file
    input: Box<dyn TokenIo<Item = char>>,
    option: CompileOption,
    context: Context,
}

impl CompilerImpl {
    #[inline]
    pub fn report_compiler_error<T>(&self, info: ErrorInfo) -> RuntimeResult<T> {
        Err(RuntimeError::new(Box::new(self.context.clone()), info))
    }
}

pub struct Compiler {
    pub compiler_impl: Rc<RefCell<CompilerImpl>>,
}

impl Compiler {
    pub fn new(option: CompileOption) -> Self {
        match option.inputsource {
            InputSource::File(ref filename) => {
                let f = match fs::File::open(filename) {
                    Err(e) => {
                        eprintln!(
                            "{}:{}",
                            t!(
                                "common.cannot_open_file",
                                filename = filename.to_str().unwrap()
                            ),
                            e
                        );
                        exit(1);
                    }
                    Ok(file) => file,
                };
                Compiler {
                    compiler_impl: Rc::new(RefCell::new(CompilerImpl {
                        input: Box::new(FileSource::new(f)),
                        option,
                        context: Context::new(cfg::MAIN_MODULE_NAME),
                    })),
                }
            }
            _ => {
                unreachable!()
            }
        }
    }

    pub fn new_string_compiler(option: CompileOption, source: &str) -> Self {
        Compiler {
            compiler_impl: Rc::new(RefCell::new(CompilerImpl {
                input: Box::new(StringSource::new(String::from(source))),
                option,
                context: Context::new(cfg::MAIN_MODULE_NAME),
            })),
        }
    }

    pub fn modify_input(&mut self, input: Box<dyn TokenIo<Item = char>>) {
        self.compiler_impl.borrow_mut().input = input;
    }

    pub fn lex(&mut self) -> RuntimeResult<StaticData> {
        let token_lexer = Rc::new(RefCell::new(TokenLex::new(self.compiler_impl.clone())));
        let env_manager = Rc::new(RefCell::new(ModuleManager::new()));
        let mut ast_builder = ModuleUnit::new(
            token_lexer.clone(),
            self.compiler_impl.clone(),
            env_manager.clone(),
        );
        ast_builder.generate_code()?;
        env_manager.borrow_mut().add_module(
            self.compiler_impl
                .borrow()
                .option
                .inputsource
                .get_path()
                .to_string(),
            ast_builder,
        );
        if self.compiler_impl.borrow().option.optimize {
            env_manager.borrow_mut().optimize();
        }
        let mut tmp = env_manager.borrow_mut();
        Ok(tmp.link())
    }

    pub fn get_token_lex(&mut self) -> TokenLex {
        TokenLex::new(self.compiler_impl.clone())
    }
}

mod tests {
    use super::*;

    fn check_read(reader: &mut impl TokenIo<Item = char>, s: &str) {
        let mut iter = s.chars();
        for i in reader {
            assert_eq!(i, iter.next().unwrap());
        }
        assert_eq!(iter.next(), None);
    }

    #[test]
    fn test_string_literal() {
        let source = "source\np";
        let mut t = StringSource::new(String::from(source));
        let mut tmp: Vec<char> = vec![t.read(), t.read()];
        tmp.reverse();
        for i in &tmp {
            t.unread(*i);
        }
        check_read(&mut t, source)
    }

    #[test]
    fn test_file_read() {
        let test_file_path = "tests/testdata/compiler/compiler1.txt";
        let source = fs::read_to_string(test_file_path).expect("please run in root dir");
        let mut t =
            FileSource::new(fs::File::open(test_file_path).expect("please run in root dir"));
        let mut tmp: Vec<char> = vec![t.read(), t.read()];
        tmp.reverse();
        for i in &tmp {
            t.unread(*i);
        }
        check_read(&mut t, &source)
    }

    #[test]
    fn test_value_pool() {
        let mut pool = ValuePool::new();
        assert_eq!(pool.add_float("9.0".to_owned()), 0);
        assert_eq!(pool.add_float("9.0".to_owned()), 0);
        assert_eq!(pool.add_float("9.5".to_owned()), 1);
        assert_eq!(pool.add_string(String::from("value")), 0);
        assert_eq!(pool.add_string(String::from("value")), 0);
        assert_eq!(pool.add_string(String::from("vale")), 1);
        assert!((pool.id_float[0] - 9.0).abs() < 0.00001);
        assert_eq!(pool.id_str[1], "vale");
    }
}
