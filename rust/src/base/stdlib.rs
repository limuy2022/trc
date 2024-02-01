use crate::{
    base::error::{ARGUMENT_ERROR, ARGU_NUMBER, EXPECT_TYPE},
    compiler::scope::{Type, TypeAllowNull},
    tvm::{stdlib::prelude::*, DynaData},
};
use lazy_static::lazy_static;
use std::{
    collections::{HashMap, HashSet},
    sync::Mutex,
};

use super::{
    codegen::{Inst, Opcode},
    error::{ErrorInfo, RunResult},
};

type StdlibFunc = fn(DynaData) -> RunResult<()>;

#[derive(Hash, PartialEq, Eq, Clone, Debug)]
pub struct RustFunction {
    pub name: String,
    pub buildin_id: usize,
    pub ptr: StdlibFunc,
    pub argvs_type: Vec<Type>,
    pub return_type: TypeAllowNull,
}

pub trait FunctionInterface {
    fn check_argvs(&self, argvs: Vec<Type>) -> Result<(), ErrorInfo>;
    fn get_return_type(&self) -> &TypeAllowNull;
}

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

lazy_static! {
    static ref STD_FUNC_ID: Mutex<usize> = Mutex::new(0);
}

impl RustFunction {
    pub fn new(
        name: String,
        ptr: StdlibFunc,
        argvs_type: Vec<Type>,
        return_type: TypeAllowNull,
    ) -> RustFunction {
        let mut lock = STD_FUNC_ID.lock().unwrap();
        let tmp = *lock;
        *lock += 1;
        RustFunction {
            name,
            buildin_id: tmp,
            ptr,
            return_type,
            argvs_type,
        }
    }
}

#[derive(Debug, Clone)]
pub struct StdlibNode {
    pub name: String,
    pub sons: HashMap<String, StdlibNode>,
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

    pub fn add_module(&mut self, name: String) -> StdlibNode {
        let ret = StdlibNode::new(name.clone());
        self.sons.insert(name, ret.clone());
        ret
    }

    pub fn add_function(&mut self, name: RustFunction) {
        self.functions.insert(name);
    }

    pub fn get_module<T: Iterator<Item = String>>(&self, mut path: T) -> Option<&StdlibNode> {
        let item = path.next();
        if item.is_none() {
            return Some(self);
        }
        let item = item.unwrap();
        return self.sons.get(&item).unwrap().get_module(path);
    }
}

pub fn init() -> StdlibNode {
    // init stdlib
    let mut stdlib = StdlibNode::new("std".to_string());
    let mut prelude = stdlib.add_module("prelude".to_string());
    prelude.add_function(RustFunction::new(
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
