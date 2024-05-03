pub mod ast;
pub mod linker;
pub mod llvm_convert;
pub mod manager;
pub mod optimizer;
pub mod scope;
pub mod token;

pub use self::{ast::ModuleUnit, manager::ModuleManager, token::TokenLex};
use crate::cfg;
use core::panic;
use libcore::*;
use logos::Source;
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
    ($func_name:ident => ($const_pool:ident, $id_pool:ident, $type:ty, $convert_func: ident)) => {
        paste::paste! {
        pub fn [<add_ $func_name>](&mut self, val: $type) -> usize {
            let len_tmp = self.$const_pool.len();
            let ret = *self.$const_pool.entry(val.clone()).or_insert(len_tmp);
            if len_tmp != self.$const_pool.len() {
                self.$id_pool.push($convert_func(val));
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
        Self {
            ..Default::default()
        }
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
            self.add_float(i.to_string());
        }
        for i in &data.constpool.stringpool {
            // TODO::improve copy performance
            self.add_string(i.clone());
        }
    }
}

pub struct CompilerImpl {
    // to support read from stdin and file
    input: String,
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
                let f = match fs::read_to_string(filename) {
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
                        input: f,
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

    pub fn new_string_compiler(option: CompileOption, source: impl Into<String>) -> Self {
        Compiler {
            compiler_impl: Rc::new(RefCell::new(CompilerImpl {
                input: source.into(),
                option,
                context: Context::new(cfg::MAIN_MODULE_NAME),
            })),
        }
    }

    pub fn modify_input(&mut self, input: String) {
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
