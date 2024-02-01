use crate::{
    base::error::{ARGUMENT_ERROR, ARGU_NUMBER, EXPECT_TYPE},
    compiler::scope::{Type, TypeAllowNull},
    tvm::{stdlib::prelude::*, DynaData},
};
use downcast_rs::{impl_downcast, Downcast};
use lazy_static::lazy_static;
use std::{
    collections::{HashMap, HashSet},
    sync::{Arc, Mutex},
};

use super::error::{ErrorInfo, RunResult};

type StdlibFunc = fn(&mut DynaData) -> RunResult<()>;

#[derive(Hash, PartialEq, Eq, Clone, Debug)]
pub struct RustFunction {
    pub name: String,
    pub buildin_id: usize,
    pub ptr: StdlibFunc,
    pub argvs_type: Vec<Type>,
    pub return_type: TypeAllowNull,
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
    fn check_argvs(&self, argvs: Vec<Type>) -> Result<(), ErrorInfo>;
    fn get_return_type(&self) -> &TypeAllowNull;
}

impl Clone for Box<dyn FunctionInterface> {
    fn clone(&self) -> Self {
        self.clone_box()
    }
}

impl_downcast!(FunctionInterface);

impl FunctionInterface for RustFunction {
    fn check_argvs(&self, argvs: Vec<Type>) -> Result<(), ErrorInfo> {
        if argvs.len() != self.argvs_type.len() {
            return Err(ErrorInfo::new(
                gettextrs::gettext!(ARGU_NUMBER, self.argvs_type.len(), argvs.len()),
                gettextrs::gettext(ARGUMENT_ERROR),
            ));
        }
        for i in 0..self.argvs_type.len() {
            if argvs[i] != self.argvs_type[i] && self.argvs_type[i] != Type::Any {
                return Err(ErrorInfo::new(
                    gettextrs::gettext!(EXPECT_TYPE, self.argvs_type[i], argvs[i]),
                    gettextrs::gettext(ARGUMENT_ERROR),
                ));
            }
        }
        Ok(())
    }

    fn get_return_type(&self) -> &TypeAllowNull {
        &self.return_type
    }
}

pub struct RustClass {
    pub name: String,
    pub members: HashMap<String, String>,
    pub functions: HashSet<RustFunction>,
}

pub static mut STD_FUNC_TABLE: Vec<StdlibFunc> = vec![];

impl RustFunction {
    pub fn new(
        name: String,
        ptr: StdlibFunc,
        argvs_type: Vec<Type>,
        return_type: TypeAllowNull,
    ) -> RustFunction {
        unsafe {
            STD_FUNC_TABLE.push(ptr);
            RustFunction {
                name,
                buildin_id: STD_FUNC_TABLE.len() - 1,
                ptr,
                return_type,
                argvs_type,
            }
        }
    }
}

#[derive(Debug, Clone)]
pub struct StdlibNode {
    pub name: String,
    pub sons: HashMap<String, Arc<Mutex<StdlibNode>>>,
    pub functions: HashSet<RustFunction>,
}

impl StdlibNode {
    pub fn new(name: String) -> StdlibNode {
        StdlibNode {
            name,
            sons: HashMap::new(),
            functions: HashSet::new(),
        }
    }

    pub fn add_module(&mut self, name: String) -> Arc<Mutex<StdlibNode>> {
        let ret = Arc::new(Mutex::new(StdlibNode::new(name.clone())));
        self.sons.insert(name.clone(), ret.clone());
        ret
    }

    pub fn add_function(&mut self, name: RustFunction) {
        self.functions.insert(name);
    }

    pub fn get_module<T: Iterator<Item = String>>(&self, mut path: T) -> Option<StdlibNode> {
        let item = path.next();
        if item.is_none() {
            return Some(self.clone());
        }
        let item = item.unwrap();
        let lock = self.sons.get(&item).unwrap().lock().unwrap();
        return lock.get_module(path);
    }
}

pub fn init() -> StdlibNode {
    // init stdlib
    let mut stdlib = StdlibNode::new("std".to_string());
    let prelude = stdlib.add_module("prelude".to_string());
    prelude.lock().unwrap().add_function(RustFunction::new(
        "print".to_string(),
        tvm_print,
        vec![Type::Any],
        TypeAllowNull::No,
    ));
    stdlib
}

lazy_static! {
    pub static ref STDLIB_LIST: StdlibNode = init();
}
