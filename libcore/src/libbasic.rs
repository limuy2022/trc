use crate::dynadata::DynaData;
use downcast_rs::{impl_downcast, Downcast};
use std::{
    collections::HashMap,
    fmt::{Debug, Display},
    mem::swap,
};

use super::{codegen::Opcode, error::*};

pub type RustlibFunc = fn(&mut DynaData) -> ErrorInfoResult<()>;

pub type TypeAllowNull = Option<ClassIdxId>;
// 定义函数的索引类型
crate::impl_newtype_int!(FuncIdx, usize);
crate::impl_newtype_int!(ClassIdxId, usize);
crate::impl_newtype_int!(VarIdx, usize);
crate::impl_newtype_int!(ConstPoolData, usize);
crate::impl_newtype_int!(ScopeAllocId, usize);

#[derive(Clone, Debug)]
pub struct IOType {
    // 如果有这个参数，代表在函数末尾是可变参数
    pub var_params: bool,
    pub argvs_type: Argvs,
    pub return_type: TypeAllowNull,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum OverrideOperations {
    Add,
    Not,
    And,
    Or,
    Sub,
    SelfNegative,
    GreaterEqual,
    Greater,
    LessEqual,
    Less,
    Equal,
    NotEqual,
    Mul,
    Div,
    ExactDiv,
    Mod,
    BitNot,
    BitRightShift,
    BitLeftShift,
    BitAnd,
    BitOr,
    Xor,
    Power,
}

pub type ArgsNameTy = Vec<ConstPoolData>;
pub type Argvs = Vec<ClassIdxId>;

#[derive(Clone, Debug)]
pub struct RustFunction {
    pub name: String,
    pub buildin_id: FuncIdx,
    pub ptr: RustlibFunc,
    pub io: IOType,
}

pub struct ArgumentError {
    pub expected: usize,
    pub actual: usize,
}

impl ArgumentError {
    pub fn new(expected: usize, actual: usize) -> ArgumentError {
        ArgumentError { expected, actual }
    }
}

pub enum ArguError {
    TypeNotMatch(ArgumentError),
    NumNotMatch(ArgumentError),
}

impl IOType {
    pub fn new(
        argvs_type: Vec<ClassIdxId>,
        return_type: TypeAllowNull,
        var_params: bool,
    ) -> IOType {
        IOType {
            argvs_type,
            return_type,
            var_params,
        }
    }

    pub fn check_argvs(&self, argvs: Argvs) -> Result<(), ArguError> {
        if argvs.len() != self.argvs_type.len() {
            return Err(ArguError::NumNotMatch(ArgumentError::new(
                self.argvs_type.len(),
                argvs.len(),
            )));
        }
        for i in argvs.iter().enumerate().take(self.argvs_type.len()) {
            // if self.argvs_type[i.0] == 0 {
            //     continue;
            // }
            if self.argvs_type[i.0] != *i.1 {
                return Err(ArguError::TypeNotMatch(ArgumentError::new(
                    *self.argvs_type[i.0],
                    **i.1,
                )));
            }
        }
        Ok(())
    }
}

pub trait FunctionInterface: Downcast + Debug {
    fn get_io(&self) -> &IOType;
    fn get_name(&self) -> &str;
    fn get_io_mut(&mut self) -> &mut IOType;
    fn get_func_id(&self) -> FuncIdx;
}

impl_downcast!(FunctionInterface);

pub trait ClassInterface: Downcast + Sync + Send + Debug + Display {
    fn has_func(&self, funcname: &str) -> Option<Box<dyn FunctionInterface>>;

    fn has_attr(&self, attrname: ConstPoolData) -> bool;

    fn get_id(&self) -> ClassIdxId;

    fn get_name(&self) -> &str;

    fn is_any(&self) -> bool {
        *self.get_id() == 0
    }

    fn get_override_func(&self, oper_token: OverrideOperations) -> Option<&OverrideWrapper>;
}

impl_downcast!(ClassInterface);

impl FunctionInterface for RustFunction {
    fn get_io(&self) -> &IOType {
        &self.io
    }

    fn get_name(&self) -> &str {
        &self.name
    }

    fn get_io_mut(&mut self) -> &mut IOType {
        &mut self.io
    }

    fn get_func_id(&self) -> FuncIdx {
        self.buildin_id
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

#[derive(Debug, Clone, Default)]
pub struct RustClass {
    /// 成员变量
    pub attribute_members: HashMap<String, String>,
    /// 成员函数
    pub functions: HashMap<String, RustFunction>,
    /// 重载函数
    pub overrides: HashMap<OverrideOperations, OverrideWrapper>,
    /// 类ID
    pub id: ClassIdxId,
    /// 类名
    pub name: &'static str,
    pub id_to_var: HashMap<ConstPoolData, ClassIdxId>,
}

/// 约定，0号id是any类型
impl RustClass {
    pub fn new_in_storage(
        name: &'static str,
        members: HashMap<String, String>,
        functions: Option<HashMap<String, RustFunction>>,
        overrides: Option<HashMap<OverrideOperations, OverrideWrapper>>,
        storage: &mut ModuleStorage,
    ) -> ClassIdxId {
        let ret = RustClass {
            attribute_members: members,
            functions: functions.unwrap_or_default(),
            overrides: overrides.unwrap_or_default(),
            id: ClassIdxId(storage.class_table.len()),
            name,
            ..Default::default()
        };
        storage.add_class(ret)
    }

    pub fn add_function(&mut self, name: impl Into<String>, func: RustFunction) {
        self.functions.insert(name.into(), func);
    }

    pub fn add_attr(&mut self, name: impl Into<String>, ty: String) {
        self.attribute_members.insert(name.into(), ty);
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

    fn has_attr(&self, attrname: ConstPoolData) -> bool {
        self.id_to_var.contains_key(&attrname)
    }

    fn get_id(&self) -> ClassIdxId {
        self.id
    }

    fn get_name(&self) -> &str {
        self.name
    }

    fn get_override_func(&self, oper_token: OverrideOperations) -> Option<&OverrideWrapper> {
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

/// 最基础的储存一个动态链接库中的函数和类的地方
#[derive(Default)]
#[repr(C)]
pub struct ModuleStorage {
    pub(crate) func_table: Vec<RustlibFunc>,
    pub(crate) class_table: Vec<RustClass>,
}

impl ModuleStorage {
    pub fn new() -> Self {
        Self {
            func_table: Vec::new(),
            class_table: Vec::new(),
        }
    }

    pub fn add_func(&mut self, f: RustlibFunc) -> FuncIdx {
        self.func_table.push(f);
        FuncIdx(self.func_table.len() - 1)
    }

    /// 获取类的个数
    pub fn get_class_end(&self) -> usize {
        self.class_table.len()
    }

    /// 添加类,获取新添加类的ID
    pub fn add_class(&mut self, c: RustClass) -> ClassIdxId {
        self.class_table.push(c);
        ClassIdxId(self.class_table.len() - 1)
    }

    pub fn access_func(&self, id: FuncIdx) -> RustlibFunc {
        self.func_table[*id]
    }

    pub fn access_class(&self, id: ClassIdxId) -> &RustClass {
        &self.class_table[*id]
    }

    pub fn func_table(&self) -> &[RustlibFunc] {
        &self.func_table
    }

    pub fn access_class_mut(&mut self, idx: ClassIdxId) -> &mut RustClass {
        &mut self.class_table[*idx]
    }
}

impl RustFunction {
    pub fn new(
        name: impl Into<String>,
        ptr: RustlibFunc,
        io: IOType,
        storage: &mut ModuleStorage,
    ) -> RustFunction {
        let buildin_id = storage.add_func(ptr);
        Self {
            name: name.into(),
            buildin_id,
            ptr,
            io,
        }
    }
}

pub type Classes = HashMap<String, ClassIdxId>;

#[derive(Debug, Clone)]
#[repr(C)]
pub struct Module {
    name: String,
    sub_modules: HashMap<String, Module>,
    functions: Vec<(String, RustFunction)>,
    name_map_index: HashMap<String, usize>,
    // class name 和class id
    classes: Classes,
    consts: HashMap<String, String>,
}

impl Module {
    pub fn new(
        name: impl Into<String>,
        sub_modules: HashMap<String, Module>,
        functions: Vec<(String, RustFunction)>,
        classes: Classes,
        consts: HashMap<String, String>,
    ) -> Module {
        let mut ret = Module {
            name: name.into(),
            sub_modules,
            functions,
            classes,
            consts,
            name_map_index: HashMap::new(),
        };
        let mut tmp = vec![];
        swap(&mut tmp, &mut ret.functions);
        for (counter, i) in tmp.iter_mut().enumerate() {
            ret.add_funcmap(i.0.clone(), counter);
        }
        swap(&mut tmp, &mut ret.functions);
        ret
    }

    pub fn add_module(&mut self, name: String, module: Module) {
        let ret = self.sub_modules.insert(name, module);
        debug_assert!(ret.is_none());
    }

    pub fn add_funcmap(&mut self, name: String, id: usize) {
        self.name_map_index.insert(name, id);
    }

    pub fn get_func_id_by_name(&self, name: &str) -> Option<usize> {
        self.name_map_index.get(name).cloned()
    }

    pub fn get_module<T: Iterator<Item = impl Into<String>>>(&self, mut path: T) -> Option<Module> {
        let item = path.next();
        if item.is_none() {
            return Some(self.clone());
        }
        let item = item.unwrap();
        let lock = self.sub_modules.get(&item.into()).unwrap();
        lock.get_module(path)
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn set_name(&mut self, name: String) {
        self.name = name;
    }

    pub fn sub_modules(&self) -> &HashMap<String, Module> {
        &self.sub_modules
    }

    pub fn functions(&self) -> &Vec<(String, RustFunction)> {
        &self.functions
    }

    pub fn classes(&self) -> &Classes {
        &self.classes
    }

    pub fn consts(&self) -> &HashMap<String, String> {
        &self.consts
    }
}

pub const INT: &str = "int";
pub const FLOAT: &str = "float";
pub const BOOL: &str = "bool";
pub const CHAR: &str = "char";
pub const STR: &str = "str";
