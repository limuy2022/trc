use super::error::*;
use crate::{
    base::error::{ARGUMENT_ERROR, ARGU_NUMBER, EXPECT_TYPE},
    compiler::{
        scope::{Type, TypeAllowNull, Var},
        token::TokenType,
    },
    tvm::DynaData,
};
use downcast_rs::{impl_downcast, Downcast};
use lazy_static::lazy_static;
use std::{
    cell::RefCell,
    collections::HashMap,
    fmt::{Debug, Display},
};

type StdlibFunc = fn(&mut DynaData) -> RuntimeResult<()>;

#[derive(Clone, Debug)]
pub struct IOType {
    pub argvs_type: Vec<RustClass>,
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
    pub fn new(argvs_type: Vec<RustClass>, return_type: TypeAllowNull) -> IOType {
        IOType {
            argvs_type,
            return_type,
        }
    }

    pub fn check_argvs(&self, argvs: Vec<Type>) -> Result<(), ErrorInfo> {
        if argvs.len() != self.argvs_type.len() {
            return Err(ErrorInfo::new(
                gettextrs::gettext!(ARGU_NUMBER, self.argvs_type.len(), argvs.len()),
                gettextrs::gettext(ARGUMENT_ERROR),
            ));
        }
        for i in 0..self.argvs_type.len() {
            if self.argvs_type[i].is_any() {
                continue;
            }
            if self.argvs_type[i].get_id() != argvs[i].get_id() {
                return Err(ErrorInfo::new(
                    gettextrs::gettext!(EXPECT_TYPE, self.argvs_type[i], argvs[i]),
                    gettextrs::gettext(ARGUMENT_ERROR),
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

    fn get_name(&self) -> &str;

    fn get_id(&self) -> usize;

    fn is_any(&self) -> bool {
        self.get_id() == 0
    }
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
pub struct RustClass {
    pub name: String,
    pub members: HashMap<String, Var>,
    pub functions: HashMap<String, RustFunction>,
    pub overrides: HashMap<TokenType, IOType>,
    pub id: usize,
}

/// 约定，0号id是any类型
impl RustClass {
    pub fn new(
        name: impl Into<String>,
        members: HashMap<String, Var>,
        functions: HashMap<String, RustFunction>,
        overrides: HashMap<TokenType, IOType>,
    ) -> RustClass {
        RustClass {
            name: name.into(),
            members,
            functions,
            overrides,
            id: 0,
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

    fn get_name(&self) -> &str {
        &self.name
    }

    fn get_id(&self) -> usize {
        self.id
    }

    fn has_attr(&self, attrname: &str) -> Option<Type> {
        let ret = &self.members.get(attrname);
        match ret {
            Some(i) => Some(i.ty.clone()),
            None => None,
        }
    }
}

impl Display for RustClass {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.name)
    }
}

thread_local! {
    pub static STD_FUNC_TABLE: RefCell<Vec<StdlibFunc>> = RefCell::new(vec![]);
}

impl RustFunction {
    pub fn new(name: impl Into<String>, ptr: StdlibFunc, io: IOType) -> RustFunction {
        Self {
            name: name.into(),
            buildin_id: STD_FUNC_TABLE.with(|std| {
                std.borrow_mut().push(ptr);
                std.borrow().len()
            }) - 1,
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
    pub classes: HashMap<String, RustClass>,
}

impl Stdlib {
    pub fn new(
        name: impl Into<String>,
        sub_modules: HashMap<String, Stdlib>,
        functions: HashMap<String, RustFunction>,
        classes: HashMap<String, RustClass>,
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
        return lock.get_module(path);
    }
}

lazy_static! {
    pub static ref ANY_TYPE: RustClass =
        RustClass::new("any", HashMap::new(), HashMap::new(), HashMap::new());
    pub static ref STDLIB_ROOT: Stdlib = crate::tvm::stdlib::init();
}
