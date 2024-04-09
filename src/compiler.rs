pub mod ast;
pub mod linker;
pub mod llvm_convent;
pub mod scope;
pub mod token;

use self::{ast::AstBuilder, token::TokenLex};
use crate::cfg;
use libcore::*;
use rust_i18n::t;
use std::{
    collections::HashMap,
    fmt::Display,
    fs,
    io::{self, BufRead},
    process::exit,
    vec,
};

#[derive(Debug, Clone)]
/// 不同的输入源
pub enum InputSource {
    File(String),
    StringInternal,
}

/// 编译器的参数控制
pub struct CompileOption {
    optimize: bool,
    inputsource: InputSource,
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
        }
    }
}

#[derive(Hash, Eq, PartialEq, Clone, Debug)]
/// # Reference
/// float hash map:<https://www.soinside.com/question/tUJxYmevbVSHZYe2C2AK5o>
pub struct Float {
    // 小数点前的部分
    front: i64,
    // 小数点后的部分
    back: i64,
    // 小数点后紧跟着的0的个数
    zero: usize,
}

impl Float {
    fn new(front: i64, back: i64, zero: usize) -> Self {
        Self { front, back, zero }
    }

    fn get_len(mut tmp: i64) -> u8 {
        if tmp == 0 {
            return 1;
        }
        let mut ret: u8 = 0;
        while tmp != 0 {
            tmp /= 10;
            ret += 1;
        }
        ret
    }

    pub fn to_float(&self) -> f64 {
        let len = Self::get_len(self.back);
        let mut float_part = self.back as f64;
        for _ in 0..len {
            float_part /= 10.0;
        }
        for _ in 0..self.zero {
            float_part /= 10.0;
        }
        self.front as f64 + float_part
    }
}

impl Display for Float {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

type Pool<T> = HashMap<T, usize>;

#[derive(Default)]
pub struct ValuePool {
    const_ints: Pool<i64>,
    const_strings: Pool<String>,
    const_floats: Pool<Float>,
    name_pool: Pool<String>,
    _const_big_int: Pool<String>,
    pub id_int: Vec<i64>,
    pub id_float: Vec<Float>,
    pub id_str: Vec<String>,
    pub id_name: Vec<String>,
}

pub const INT_VAL_POOL_ZERO: usize = 0;
pub const INT_VAL_POOL_ONE: usize = 1;

macro_rules! gen_getter_setter {
    ($($func_name:ident => ($const_pool:ident, $id_pool:ident, $type:ty)),*) => {
        $(
            paste::paste!{
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
        )*
    };
}

impl ValuePool {
    fn new() -> Self {
        let mut ret = Self {
            ..Default::default()
        };
        ret.add_int(0);
        ret.add_int(1);
        ret
    }

    gen_getter_setter!(
        int => (const_ints, id_int, i64),
        float => (const_floats, id_float, Float),
        string => (const_strings, id_str, String),
        id => (name_pool, id_name, String)
    );

    fn store_val_to_vm(&mut self) -> ConstPool {
        let mut ret = ConstPool::new();
        ret.intpool.resize(self.const_ints.len(), 0);
        for i in &self.const_ints {
            ret.intpool[*i.1] = *i.0;
        }
        ret.floatpool.resize(self.const_floats.len(), 0.0);
        for i in &self.const_floats {
            ret.floatpool[*i.1] = i.0.to_float();
        }
        ret.stringpool
            .resize(self.const_strings.len(), "".to_string());
        for i in &self.const_strings {
            ret.stringpool[*i.1].clone_from(i.0);
        }
        ret
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

    fn next(&mut self) -> std::option::Option<Self::Item> {
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
    fn next(&mut self) -> std::option::Option<Self::Item> {
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

pub struct Compiler {
    // to support read from stdin and file
    input: Box<dyn TokenIo<Item = char>>,
    option: CompileOption,
    context: Context,
}

impl Compiler {
    pub fn new(option: CompileOption) -> Self {
        match option.inputsource {
            InputSource::File(ref filename) => {
                let f = match fs::File::open(filename) {
                    Err(e) => {
                        eprintln!(
                            "{}:{}",
                            t!("common.cannot_open_file", filename = filename),
                            e
                        );
                        exit(1);
                    }
                    Ok(file) => file,
                };
                Compiler {
                    input: Box::new(FileSource::new(f)),
                    option,
                    context: Context::new(cfg::MAIN_MODULE_NAME),
                }
            }
            _ => {
                unreachable!()
            }
        }
    }

    pub fn new_string_compiler(option: CompileOption, source: &str) -> Self {
        Compiler {
            input: Box::new(StringSource::new(String::from(source))),
            option,
            context: Context::new(cfg::MAIN_MODULE_NAME),
        }
    }

    pub fn modify_input(&mut self, input: Box<dyn TokenIo<Item = char>>) {
        self.input = input;
    }

    pub fn lex(&mut self) -> RuntimeResult<AstBuilder> {
        let token_lexer = TokenLex::new(self);
        let mut ast_builder = AstBuilder::new(token_lexer);
        ast_builder.generate_code()?;
        Ok(ast_builder)
    }

    pub fn get_token_lex(&mut self) -> TokenLex {
        let token_lexer = TokenLex::new(self);
        token_lexer
    }

    #[inline]
    pub fn report_compiler_error<T>(&self, info: ErrorInfo) -> RuntimeResult<T> {
        Err(RuntimeError::new(Box::new(self.context.clone()), info))
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
        assert_eq!(pool.add_int(7), 2);
        assert_eq!(pool.add_int(1), INT_VAL_POOL_ONE);
        assert_eq!(pool.add_int(0), INT_VAL_POOL_ZERO);
        assert_eq!(pool.add_float(Float::new(9, 0, 0)), 0);
        assert_eq!(pool.add_float(Float::new(9, 0, 0)), 0);
        assert_eq!(pool.add_float(Float::new(9, 5, 0)), 1);
        assert_eq!(pool.add_string(String::from("value")), 0);
        assert_eq!(pool.add_string(String::from("value")), 0);
        assert_eq!(pool.add_string(String::from("vale")), 1);
        assert_eq!(pool.id_int[0], 0);
        assert_eq!(pool.id_float[0], Float::new(9, 0, 0));
        assert_eq!(pool.id_str[1], "vale");
    }

    #[test]
    fn test_float() {
        assert_eq!(Float::new(9, 0, 0).to_float(), 9f64);
        assert_eq!(Float::new(9, 1, 0).to_float(), 9.1f64);
        assert_eq!(Float::new(9, 5, 1).to_float(), 9.05f64);
    }
}
