use super::{token::Token, ValuePool};
use libcore::*;
use std::{
    cell::RefCell,
    collections::HashMap,
    fmt::Display,
    rc::{Rc, Weak},
};

/// Manager of function
#[derive(Clone, Debug)]
pub struct CustomFunction {
    io: IOType,
    pub args_names: ArgsNameTy,
    pub custom_id: FuncIdx,
    name: String,
}

#[derive(thiserror::Error, Debug)]
pub enum ScopeError {
    #[error("key redefine")]
    Redefine,
}

impl CustomFunction {
    pub fn new(io: IOType, args_names: ArgsNameTy, name: impl Into<String>) -> Self {
        Self {
            io,
            args_names,
            name: name.into(),
            custom_id: FuncIdx::default(),
        }
    }

    pub fn add_argv() {}
}

impl FunctionInterface for CustomFunction {
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
        self.custom_id
    }
}

/// Manager of type
#[derive(Clone, Debug, Default)]
pub struct CustomType {
    funcs: Vec<CustomFunction>,
    pub name: ClassIdxId,
    pub origin_name: String,
    pub id_to_attr: HashMap<ConstPoolData, ClassIdxId>,
}

impl Display for CustomType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.origin_name)
    }
}

impl CustomType {
    pub fn new(name: ClassIdxId, origin_name: impl Into<String>) -> Self {
        Self {
            name,
            origin_name: origin_name.into(),
            ..Default::default()
        }
    }

    pub fn add_attr(&mut self, attrname: ConstPoolData, attrty: ClassIdxId) -> Option<ClassIdxId> {
        self.id_to_attr.insert(attrname, attrty)
    }

    pub fn add_func(&mut self, f: CustomFunction) {
        self.funcs.push(f);
    }
}

impl ClassInterface for CustomType {
    fn has_func(&self, funcname: &str) -> Option<Box<dyn FunctionInterface>> {
        for i in &self.funcs {
            if i.get_name() == funcname {
                return Some(Box::new(i.clone()));
            }
        }
        None
    }

    fn has_attr(&self, attrname: ConstPoolData) -> bool {
        self.id_to_attr.contains_key(&attrname)
    }

    fn get_id(&self) -> ClassIdxId {
        self.name
    }

    fn get_name(&self) -> &str {
        &self.origin_name
    }

    fn get_override_func(&self, _oper_token: OverrideOperations) -> Option<&OverrideWrapper> {
        None
    }
}

pub type FuncBodyTy = Vec<(Token, usize)>;

#[derive(Clone, Debug, Copy)]
pub struct VarInfo {
    pub ty: ClassIdxId,
    pub var_idx: usize,
    pub addr: usize,
}

impl VarInfo {
    pub fn new(ty: ClassIdxId, var_idx: usize, addr: usize) -> Self {
        Self { ty, var_idx, addr }
    }
}

#[derive(Clone, Debug, Default)]
pub struct RootInfo {
    types_custom_store: HashMap<ClassIdxId, Rc<dyn ClassInterface>>,
    // 当前作用域可以分配的下一个class id
    types_id: ClassIdxId,
    imported_native: HashMap<String, Rc<libloading::Library>>,
}

impl RootInfo {
    pub fn get_class_by_class_id(&self, id: ClassIdxId) -> Option<Rc<dyn ClassInterface>> {
        // 不存在的类
        if id >= self.types_id {
            return None;
        }
        self.types_custom_store.get(&id).cloned()
    }

    fn alloc_type_id(&mut self) -> Result<ClassIdxId, ScopeError> {
        let ret = self.types_id;
        *self.types_id += 1;
        Ok(ret)
    }

    fn insert_type(&mut self, id: ClassIdxId, f: Rc<dyn ClassInterface>) -> Result<(), ScopeError> {
        self.types_custom_store.insert(id, f);
        Ok(())
    }

    pub fn add_imported_native_dll(&mut self, dll_name: String, lib: Rc<libloading::Library>) {
        self.imported_native.insert(dll_name, lib);
    }
}

#[derive(Debug, Clone)]
pub enum RootOnlyInfo<T> {
    NonRoot(T),
    Root(RootInfo),
}

impl<T> Default for RootOnlyInfo<T> {
    fn default() -> Self {
        Self::Root(RootInfo::default())
    }
}

impl<T> RootOnlyInfo<T> {
    pub fn get_info(&mut self) -> &mut RootInfo {
        match self {
            RootOnlyInfo::NonRoot(_) => panic!("without root info"),
            RootOnlyInfo::Root(v) => v,
        }
    }

    pub fn unwrap(self) -> T {
        match self {
            RootOnlyInfo::NonRoot(v) => v,
            RootOnlyInfo::Root(_) => panic!("without root info"),
        }
    }
}

#[derive(Default)]
pub struct SymScope {
    // 父作用域
    pub prev_scope: RootOnlyInfo<Rc<RefCell<SymScope>>>,
    // 管理符号之间的映射,由token在name pool中的id映射到符号表中的id
    sym_map: HashMap<ConstPoolData, ScopeAllocId>,
    // 当前作用域要分配的下一个ID,也就是当前作用域的最大id+1
    scope_sym_id: ScopeAllocId,
    // ID到class id的映射
    types: HashMap<ScopeAllocId, ClassIdxId>,
    // ID到函数的映射
    funcs: HashMap<ScopeAllocId, Rc<dyn FunctionInterface>>,
    // id到变量类型的映射
    vars: HashMap<ScopeAllocId, VarInfo>,
    // 由token id到模块的映射
    // modules: HashMap<ScopeAllocIdTy, &'static Stdlib>,
    vars_id: ScopeAllocId,
    // 作用域暂时储存的函数token
    pub funcs_temp_store: Vec<(ScopeAllocId, Vec<(Token, usize)>)>,
    // 计算当前需要最少多大的空间来保存变量
    pub var_sz: usize,
    // 保存当前环境下的function io，global环境无
    pub func_io: Option<TypeAllowNull>,
    // 保存当前for的所有break
    pub for_break: Vec<usize>,
    // 保存当前for所有的continue
    pub for_continue: Vec<usize>,
    pub in_loop: bool,
    pub in_class: bool,
    pub is_pub: bool,
    imported_modules: HashMap<ScopeAllocId, Rc<RefCell<SymScope>>>,
    root_scope: Option<Weak<RefCell<SymScope>>>,
}

pub enum SymScopePrev {
    Prev(Rc<RefCell<SymScope>>),
    Root,
}

impl SymScope {
    pub fn new(prev_scope: SymScopePrev) -> Self {
        match prev_scope {
            SymScopePrev::Prev(prev_scope) => {
                let mut ret = Self {
                    prev_scope: RootOnlyInfo::NonRoot(prev_scope.clone()),
                    ..Default::default()
                };
                match prev_scope.borrow().root_scope.clone() {
                    Some(v) => ret.root_scope = Some(v),
                    None => ret.root_scope = Some(Rc::downgrade(&prev_scope)),
                }
                ret.scope_sym_id = prev_scope.borrow().scope_sym_id;
                ret
            }
            SymScopePrev::Root => Self {
                root_scope: None,
                prev_scope: RootOnlyInfo::Root(Default::default()),
                ..Default::default()
            },
        }
    }

    pub fn add_custom_function(
        &mut self,
        id: ScopeAllocId,
        function_id: FuncIdx,
        mut f: CustomFunction,
        body: Vec<(Token, usize)>,
    ) {
        self.funcs_temp_store.push((id, body));
        f.custom_id = function_id;
        self.add_func(id, Rc::new(f));
    }

    pub fn insert_sym_with_error(
        &mut self,
        sym_name: ConstPoolData,
        str_name: &str,
    ) -> Result<ScopeAllocId, ErrorInfo> {
        match self.insert_sym(sym_name) {
            Some(v) => Ok(v),
            None => Err(symbol_redefined(str_name)),
        }
    }

    pub fn add_imported_module(&mut self, id: ScopeAllocId, son_modules: Rc<RefCell<SymScope>>) {
        self.imported_modules.insert(id, son_modules);
    }

    fn fix_func(&self, io: &mut IOType, storage: &ModuleStorage, pool: &ValuePool) {
        for i in &mut io.argvs_type {
            *i = self
                .get_type_id_by_token(pool.name_pool[storage.access_class(*i).name])
                .unwrap();
        }
        match io.return_type {
            None => {}
            Some(t) => {
                io.return_type = Some(
                    self.get_type_id_by_token(pool.name_pool[storage.access_class(t).name])
                        .unwrap(),
                );
            }
        }
    }

    /// import the module defined in rust
    /// # Warning
    /// 第一次导入dll记得将id加dll中函数的个数个
    pub fn import_native_module(
        &mut self,
        library: &Module,
        libstorage: &ModuleStorage,
        const_pool: &ValuePool,
        prev_func_base: usize, // 给定函数索引起始点
    ) -> Result<(), ErrorInfo> {
        let types = library.classes();
        // println!("{:?}", types);
        let mut obj_vec = vec![];
        for i in types {
            let idx = self.insert_sym_with_error(const_pool.name_pool[i.0], i.0)?;
            let obj_rc = libstorage.access_class(*i.1).clone();
            let classid = match self.alloc_type_id(idx) {
                Err(_) => return Err(symbol_redefined(i.0)),
                Ok(t) => t,
            };
            obj_vec.push((classid, obj_rc));
        }
        // 改写类的重载方法
        for i in &mut obj_vec {
            for j in &mut i.1.functions {
                self.fix_func(j.1.get_io_mut(), libstorage, const_pool);
            }
            for j in &mut i.1.overrides {
                self.fix_func(&mut j.1.io, libstorage, const_pool)
            }
        }
        for i in obj_vec {
            self.insert_type(i.0, Rc::new(i.1))
                .expect("type symbol conflict");
        }
        let funcs = library.functions();
        for i in funcs {
            let idx = self.insert_sym_with_error(const_pool.name_pool[&i.0], &i.0)?;
            // 在将类型全部添加进去之后，需要重新改写函数和类的输入和输出参数
            let mut fobj = i.1.clone();
            self.fix_func(fobj.get_io_mut(), libstorage, const_pool);
            self.add_extern_func(idx, FuncIdx(prev_func_base + *fobj.buildin_id), fobj);
        }
        Ok(())
    }

    pub fn get_module(&self, id: ScopeAllocId) -> Option<Rc<RefCell<SymScope>>> {
        let t = self.imported_modules.get(&id);
        match t {
            None => match self.prev_scope {
                RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope.borrow().get_module(id),
                RootOnlyInfo::Root(_) => None,
            },
            Some(v) => Some(v.clone()),
        }
    }

    pub fn get_function(&self, id: ScopeAllocId) -> Option<Rc<dyn FunctionInterface>> {
        match self.funcs.get(&id) {
            Some(f) => Some(f.clone()),
            None => match self.prev_scope {
                RootOnlyInfo::NonRoot(ref prev) => prev.borrow().get_function(id),
                RootOnlyInfo::Root(_) => None,
            },
        }
    }

    pub fn has_sym(&self, id: ConstPoolData) -> bool {
        if self.sym_map.contains_key(&id) {
            return true;
        }
        match self.prev_scope {
            RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope.borrow().has_sym(id),
            RootOnlyInfo::Root(_) => false,
        }
    }

    pub fn new_id(&mut self) -> usize {
        let ret = self.scope_sym_id;
        self.scope_sym_id += 1;
        ret
    }

    pub fn insert_sym(&mut self, id: ConstPoolData) -> Option<usize> {
        let t = self.sym_map.insert(id, self.scope_sym_id);
        self.scope_sym_id += 1;
        // 先前不能存在
        match t {
            None => Some(self.scope_sym_id - 1),
            Some(_) => None,
        }
    }

    /// 返回变量的类型，索引和内存地址
    pub fn get_var(&self, id: ScopeAllocId) -> Option<VarInfo> {
        match self.vars.get(&id) {
            Some(v) => Some(*v),
            None => match self.prev_scope {
                RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope.borrow().get_var(id),
                RootOnlyInfo::Root(_) => None,
            },
        }
    }

    pub fn get_sym(&self, id: ConstPoolData) -> Option<usize> {
        let t = self.sym_map.get(&id);
        match t {
            None => match self.prev_scope {
                RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope.borrow().get_sym(id),
                RootOnlyInfo::Root(_) => None,
            },
            Some(t) => Some(*t),
        }
    }

    fn add_func(&mut self, id: ScopeAllocId, f: Rc<dyn FunctionInterface>) {
        self.funcs.insert(id, f);
    }

    fn add_extern_func(&mut self, id: ScopeAllocId, function_id: FuncIdx, mut f: RustFunction) {
        f.buildin_id = function_id;
        self.add_func(id, Rc::new(f))
    }

    pub fn import_extern_func(
        &mut self,
        mut f: RustFunction,
        tokenid: ConstPoolData,
        fname: &str,
        extern_function_id: FuncIdx,
        storage: &ModuleStorage,
        pool: &ValuePool,
    ) -> Result<(), ErrorInfo> {
        // println!("{}", f.get_name());
        let func_id = self.insert_sym_with_error(tokenid, fname)?;
        self.fix_func(f.get_io_mut(), storage, pool);
        self.add_extern_func(func_id, extern_function_id, f.clone());
        Ok(())
    }

    /// 返回变量的索引和内存地址
    pub fn add_var(&mut self, id: ScopeAllocId, ty: ClassIdxId, var_sz: usize) -> (VarIdx, usize) {
        let ret_addr = self.var_sz;
        self.vars
            .insert(id, VarInfo::new(ty, self.vars_id, ret_addr));
        let ret = self.vars_id;
        self.vars_id += 1;
        self.var_sz += var_sz;
        (VarIdx(ret), ret_addr)
    }

    pub fn get_var_table_sz(&self) -> usize {
        self.var_sz
    }

    pub fn get_type_id(&self, id: ScopeAllocId) -> Option<ClassIdxId> {
        match self.types.get(&id) {
            None => match self.prev_scope {
                RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope.borrow().get_type_id(id),
                RootOnlyInfo::Root(_) => None,
            },
            Some(t) => Some(*t),
        }
    }

    pub fn get_scope_last_idx(&self) -> usize {
        self.scope_sym_id
    }

    pub fn get_var_table_len(&self) -> usize {
        self.vars_id
    }

    pub fn get_class_by_class_id(&self, classid: ClassIdxId) -> Option<Rc<dyn ClassInterface>> {
        // if classid >= self.types_id {
        //     return None;
        // }
        // self.types_custom_store.get(&classid).cloned()
        match &self.prev_scope {
            RootOnlyInfo::NonRoot(ref prev_scope) => {
                prev_scope.borrow().get_class_by_class_id(classid)
            }
            RootOnlyInfo::Root(data) => data.get_class_by_class_id(classid),
        }
    }

    pub fn get_class(&self, classid_idx: ScopeAllocId) -> Option<Rc<dyn ClassInterface>> {
        let t = self.types.get(&classid_idx);
        match t {
            Some(t) => self.get_class_by_class_id(*t),
            None => match self.prev_scope {
                RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope.borrow().get_class(classid_idx),
                RootOnlyInfo::Root(_) => None,
            },
        }
    }

    fn alloc_type_id(&mut self, idx: ScopeAllocId) -> Result<ClassIdxId, ScopeError> {
        let ret = match &mut self.prev_scope {
            RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope.borrow_mut().alloc_type_id(idx),
            RootOnlyInfo::Root(data) => data.alloc_type_id(),
        }?;
        self.types.insert(idx, ret);
        Ok(ret)
    }

    fn insert_type(&mut self, id: ClassIdxId, f: Rc<dyn ClassInterface>) -> Result<(), ScopeError> {
        match &mut self.prev_scope {
            RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope.borrow_mut().insert_type(id, f),
            RootOnlyInfo::Root(data) => data.insert_type(id, f),
        }
    }

    pub fn add_type(
        &mut self,
        idx: ScopeAllocId,
        obj: Rc<dyn ClassInterface>,
    ) -> Result<ClassIdxId, ScopeError> {
        let ret = self.alloc_type_id(idx)?;
        self.insert_type(ret, obj)?;
        Ok(ret)
    }

    pub fn get_type_id_by_token(&self, ty_name: ConstPoolData) -> Option<ClassIdxId> {
        let ty_idx_id = self.get_sym(ty_name).unwrap();
        self.get_type_id(ty_idx_id)
    }

    pub fn add_imported_native_dll(
        &mut self,
        dll_name: String,
        lib: Rc<libloading::Library>,
        storage: &ModuleStorage,
        addid: impl Fn(usize),
    ) {
        match &mut self.prev_scope {
            RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope
                .borrow_mut()
                .add_imported_native_dll(dll_name, lib, storage, addid),
            RootOnlyInfo::Root(data) => {
                data.add_imported_native_dll(dll_name, lib);
                addid(storage.func_table().len());
            }
        }
    }

    pub fn get_dll(&self, name: &str) -> Option<Rc<libloading::Library>> {
        match &self.prev_scope {
            RootOnlyInfo::NonRoot(ref prev_scope) => prev_scope.borrow().get_dll(name),
            RootOnlyInfo::Root(data) => data.imported_native.get(name).map(|v| (*v).clone()),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_scope() {
        let root_scope = Rc::new(RefCell::new(SymScope::new(SymScopePrev::Root)));
        root_scope.borrow_mut().insert_sym(ConstPoolData(1));
        let mut son_scope = SymScope::new(SymScopePrev::Prev(root_scope.clone()));
        son_scope.insert_sym(ConstPoolData(2));
        assert_eq!(son_scope.get_sym(ConstPoolData(2)), Some(1));
        drop(son_scope);
        assert_eq!(root_scope.borrow().get_sym(ConstPoolData(1)), Some(0));
    }
}
