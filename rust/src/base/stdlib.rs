use super::{codegen::Opcode, error::*};
use crate::{
    base::error::{ARGUMENT_ERROR, ARGU_NUMBER, EXPECT_TYPE},
    compiler::{
        scope::{Type, TypeAllowNull, Var},
        token::TokenType,
    },
    tvm::DynaData,
};
use downcast_rs::{impl_downcast, Downcast};
use rust_i18n::t;
use std::sync::OnceLock;
use std::{
    collections::HashMap,
    fmt::{Debug, Display},
};

type StdlibFunc = fn(&mut DynaData) -> RuntimeResult<()>;
const ANY_TYPE_ID: usize = 0;

#[derive(Clone, Debug)]
pub struct IOType {
    // 如果有这个参数，代表在函数末尾是可变参数
    pub var_params: bool,
    pub argvs_type: Vec<usize>,
    pub return_type: TypeAllowNull,
}

#[derive(Clone, Debug)]
pub struct RustFunction {
    pub name: String,
    pub buildin_id: usize,
    pub ptr: StdlibFunc,
    pub io: IOType,
}

impl IOType {
    pub fn new(argvs_type: Vec<usize>, return_type: TypeAllowNull, var_params: bool) -> IOType {
        IOType {
            argvs_type,
            return_type,
            var_params,
        }
    }

    pub fn check_argvs(&self, argvs: Vec<usize>) -> Result<(), ErrorInfo> {
        if argvs.len() != self.argvs_type.len() {
            return Err(ErrorInfo::new(
                t!(ARGU_NUMBER, "0" = self.argvs_type.len(), "1" = argvs.len()),
                t!(ARGUMENT_ERROR),
            ));
        }
        for i in argvs.iter().enumerate().take(self.argvs_type.len()) {
            if self.argvs_type[i.0] == 0 {
                continue;
            }
            if self.argvs_type[i.0] != *i.1 {
                return Err(ErrorInfo::new(
                    t!(EXPECT_TYPE, "0" = self.argvs_type[i.0], "1" = i.1),
                    t!(ARGUMENT_ERROR),
                ));
            }
        }
        Ok(())
    }
}

pub trait FunctionClone {
    fn clone_box(&self) -> Box<dyn FunctionInterface>;
}

impl<T> FunctionClone for T
where
    T: 'static + FunctionInterface + Clone,
{
    fn clone_box(&self) -> Box<dyn FunctionInterface> {
        Box::new(self.clone())
    }
}

pub trait FunctionInterface: Downcast + FunctionClone {
    fn get_io(&self) -> &IOType;
    fn get_name(&self) -> &str;
}

impl Clone for Box<dyn FunctionInterface> {
    fn clone(&self) -> Self {
        self.clone_box()
    }
}

impl_downcast!(FunctionInterface);

pub trait ClassClone {
    fn clone_box(&self) -> Box<dyn ClassInterface>;
}

pub trait ClassInterface: Downcast + Sync + Send + ClassClone + Debug + Display {
    fn has_func(&self, funcname: &str) -> Option<Box<dyn FunctionInterface>>;

    fn has_attr(&self, attrname: &str) -> Option<Type>;

    fn get_id(&self) -> usize;

    fn get_name(&self) -> &str;

    fn is_any(&self) -> bool {
        self.get_id() == 0
    }

    fn get_override_func(&self, oper_token: TokenType) -> Option<&OverrideWrapper>;
}

impl<T> ClassClone for T
where
    T: 'static + ClassInterface + Clone,
{
    fn clone_box(&self) -> Box<dyn ClassInterface> {
        Box::new(self.clone())
    }
}

impl Clone for Box<dyn ClassInterface> {
    fn clone(&self) -> Self {
        self.clone_box()
    }
}

impl_downcast!(ClassInterface);

impl FunctionInterface for RustFunction {
    fn get_io(&self) -> &IOType {
        &self.io
    }

    fn get_name(&self) -> &str {
        &self.name
    }
}

#[derive(Debug, Clone)]
pub struct OverrideWrapper {
    pub opcode: Opcode,
    pub io: IOType,
}

impl OverrideWrapper {
    pub fn new(opcode: Opcode, io: IOType) -> OverrideWrapper {
        OverrideWrapper { opcode, io }
    }
}

#[derive(Debug, Clone)]
pub struct RustClass {
    pub members: HashMap<String, Var>,
    pub functions: HashMap<String, RustFunction>,
    pub overrides: HashMap<TokenType, OverrideWrapper>,
    pub id: usize,
    pub name: &'static str,
}

/// 约定，0号id是any类型
impl RustClass {
    pub fn new(
        name: &'static str,
        members: HashMap<String, Var>,
        functions: Option<HashMap<String, RustFunction>>,
        overrides: Option<HashMap<TokenType, OverrideWrapper>>,
        id: usize,
    ) -> RustClass {
        RustClass {
            members,
            functions: functions.unwrap_or_default(),
            overrides: overrides.unwrap_or_default(),
            id,
            name,
        }
    }

    pub fn add_function(&mut self, name: impl Into<String>, func: RustFunction) {
        self.functions.insert(name.into(), func);
    }

    pub fn add_attr(&mut self, name: impl Into<String>, attr: Var) {
        self.members.insert(name.into(), attr);
    }
}

impl ClassInterface for RustClass {
    fn has_func(&self, funcname: &str) -> Option<Box<dyn FunctionInterface>> {
        for i in &self.functions {
            if i.0 == funcname {
                return Some(Box::new(i.1.clone()));
            }
        }
        None
    }

    fn has_attr(&self, attrname: &str) -> Option<Type> {
        let ret = &self.members.get(attrname);
        match ret {
            Some(i) => Some(i.ty.clone()),
            None => None,
        }
    }

    fn get_id(&self) -> usize {
        self.id
    }

    fn get_name(&self) -> &str {
        self.name
    }

    fn get_override_func(&self, oper_token: TokenType) -> Option<&OverrideWrapper> {
        match self.overrides.get(&oper_token) {
            Some(i) => Some(i),
            None => None,
        }
    }
}

impl Display for RustClass {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.name)
    }
}

pub static mut STD_FUNC_TABLE: Vec<StdlibFunc> = vec![];
pub static mut STD_CLASS_TABLE: Vec<RustClass> = vec![];

pub fn get_stdlib() -> &'static Stdlib {
    static INIT: OnceLock<Stdlib> = OnceLock::new();
    INIT.get_or_init(crate::tvm::stdlib::import_stdlib)
}

pub fn new_class_id() -> usize {
    unsafe { STD_CLASS_TABLE.len() }
}

impl RustFunction {
    pub fn new(name: impl Into<String>, ptr: StdlibFunc, io: IOType) -> RustFunction {
        let buildin_id = unsafe {
            STD_FUNC_TABLE.push(ptr);
            STD_FUNC_TABLE.len()
        } - 1;
        Self {
            name: name.into(),
            buildin_id,
            ptr,
            io,
        }
    }
}

#[derive(Debug, Clone)]
pub struct Stdlib {
    pub name: String,
    pub sub_modules: HashMap<String, Stdlib>,
    pub functions: HashMap<String, RustFunction>,
    pub classes: HashMap<String, usize>,
}

impl Stdlib {
    pub fn new(
        name: impl Into<String>,
        sub_modules: HashMap<String, Stdlib>,
        functions: HashMap<String, RustFunction>,
        classes: HashMap<String, usize>,
    ) -> Stdlib {
        Stdlib {
            name: name.into(),
            sub_modules,
            functions,
            classes,
        }
    }

    pub fn add_module(&mut self, name: String, module: Stdlib) {
        let ret = self.sub_modules.insert(name, module);
        debug_assert!(ret.is_none());
    }

    pub fn add_function(&mut self, name: String, func: RustFunction) {
        self.functions.insert(name, func);
    }

    pub fn get_module<T: Iterator<Item = String>>(&self, mut path: T) -> Option<Stdlib> {
        let item = path.next();
        if item.is_none() {
            return Some(self.clone());
        }
        let item = item.unwrap();
        let lock = self.sub_modules.get(&item).unwrap();
        lock.get_module(path)
    }
}

pub fn get_any_type() -> &'static RustClass {
    static ANY_TYPE: OnceLock<RustClass> = OnceLock::new();
    ANY_TYPE.get_or_init(|| RustClass::new("any", HashMap::new(), None, None, ANY_TYPE_ID))
}

pub struct AnyType {}

impl AnyType {
    pub fn export_info() -> usize {
        ANY_TYPE_ID
    }
}

/// 获取到标准库的类的个数，从而区分标准库和用户自定义的类
pub fn get_stdclass_end() -> usize {
    static STD_NUM: OnceLock<usize> = OnceLock::new();
    *STD_NUM.get_or_init(|| unsafe { STD_CLASS_TABLE.len() })
}
