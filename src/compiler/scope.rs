use super::{
    token::{ConstPoolIndexTy, Token},
    ValuePool,
};
use libcore::*;
use std::{cell::RefCell, collections::HashMap, fmt::Display, rc::Rc};

/// Manager of function
#[derive(Clone, Debug)]
pub struct CustomFunction {
    io: IOType,
    pub args_names: ArgsNameTy,
    pub custom_id: FuncIdxTy,
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
            custom_id: 0,
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
}

/// Manager of type
#[derive(Clone, Debug, Default)]
pub struct CustomType {
    funcs: Vec<CustomFunction>,
    pub name: usize,
    pub origin_name: String,
    pub id_to_attr: HashMap<ConstPoolIndexTy, ScopeAllocIdTy>,
}

impl Display for CustomType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.origin_name)
    }
}

impl CustomType {
    pub fn new(name: usize, origin_name: impl Into<String>) -> Self {
        Self {
            name,
            origin_name: origin_name.into(),
            ..Default::default()
        }
    }

    pub fn add_attr(&mut self, attrname: ScopeAllocIdTy, attrty: TyIdxTy) -> Option<TyIdxTy> {
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

    fn has_attr(&self, attrname: usize) -> bool {
        self.id_to_attr.contains_key(&attrname)
    }

    fn get_id(&self) -> usize {
        self.name
    }

    fn get_name(&self) -> &str {
        &self.origin_name
    }

    fn get_override_func(
        &self,
        _oper_token: libcore::OverrideOperations,
    ) -> Option<&OverrideWrapper> {
        None
    }
}

pub type ScopeAllocClassId = usize;
pub type VarIdxTy = ScopeAllocIdTy;
pub type FuncIdxTy = ScopeAllocIdTy;
pub type FuncBodyTy = Vec<(Token, usize)>;

#[derive(Clone, Debug, Copy)]
pub struct VarInfo {
    pub ty: TyIdxTy,
    pub var_idx: usize,
    pub addr: usize,
}

impl VarInfo {
    pub fn new(ty: TyIdxTy, var_idx: usize, addr: usize) -> Self {
        Self { ty, var_idx, addr }
    }
}

#[derive(Default)]
pub struct SymScope {
    // 父作用域
    pub prev_scope: Option<Rc<RefCell<SymScope>>>,
    // 管理符号之间的映射,由token在name pool中的id映射到符号表中的id
    sym_map: HashMap<ConstPoolIndexTy, ScopeAllocIdTy>,
    // 当前作用域要分配的下一个ID,也就是当前作用域的最大id+1
    scope_sym_id: ScopeAllocIdTy,
    // ID到class id的映射
    types: HashMap<ScopeAllocIdTy, ScopeAllocClassId>,
    // ID到函数的映射
    funcs: HashMap<ScopeAllocIdTy, Rc<dyn FunctionInterface>>,
    // id到变量类型的映射
    vars: HashMap<ScopeAllocIdTy, VarInfo>,
    // 由token id到模块的映射
    // modules: HashMap<ScopeAllocIdTy, &'static Stdlib>,
    // 当前作用域可以分配的下一个class id
    types_id: ScopeAllocClassId,
    vars_id: ScopeAllocIdTy,
    funcs_custom_id: ScopeAllocIdTy,
    // 用户自定义的类型储存位置
    types_custom_store: HashMap<ScopeAllocClassId, Rc<dyn ClassInterface>>,
    // 作用域暂时储存的函数token
    pub funcs_temp_store: Vec<(ScopeAllocIdTy, Vec<(Token, usize)>)>,
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
    imported_modules: HashMap<ScopeAllocIdTy, Rc<RefCell<SymScope>>>,
}

impl SymScope {
    pub fn new(prev_scope: Option<Rc<RefCell<SymScope>>>) -> Self {
        let mut ret = Self {
            prev_scope: prev_scope.clone(),
            ..Default::default()
        };
        if let Some(prev_scope) = prev_scope {
            ret.scope_sym_id = prev_scope.borrow().scope_sym_id;
            ret.types_id = prev_scope.borrow().types_id;
            ret.funcs_custom_id = prev_scope.borrow().funcs_custom_id;
        }
        ret
    }

    pub fn add_custom_function(
        &mut self,
        id: ScopeAllocIdTy,
        mut f: CustomFunction,
        body: Vec<(Token, usize)>,
    ) -> FuncIdxTy {
        let ret = self.funcs_custom_id;
        self.funcs_temp_store.push((id, body));
        f.custom_id = ret;
        self.add_func(id, Rc::new(f));
        self.funcs_custom_id += 1;
        ret
    }

    pub fn insert_sym_with_error(
        &mut self,
        sym_name: ConstPoolIndexTy,
        str_name: &str,
    ) -> Result<ScopeAllocIdTy, ErrorInfo> {
        match self.insert_sym(sym_name) {
            Some(v) => Ok(v),
            None => Err(symbol_redefined(str_name)),
        }
    }

    pub fn add_imported_module(&mut self, id: ScopeAllocIdTy, son_modules: Rc<RefCell<SymScope>>) {
        self.imported_modules.insert(id, son_modules);
    }

    fn fix_func(&self, io: &mut IOType, storage: &ModuleStorage, pool: &ValuePool) {
        for i in &mut io.argvs_type {
            *i = self
                .get_type_id_by_token(pool.name_pool[storage.class_table[*i].name])
                .unwrap();
        }
        match io.return_type {
            None => {}
            Some(t) => {
                io.return_type = Some(
                    self.get_type_id_by_token(pool.name_pool[storage.class_table[t].name])
                        .unwrap(),
                );
            }
        }
    }

    /// import the module defined in rust
    pub fn import_native_module(
        &mut self,
        stdlib: &Module,
        libstorage: &ModuleStorage,
        const_pool: &ValuePool,
    ) -> Result<(), ErrorInfo> {
        let types = &stdlib.classes;
        // println!("{:?}", types);
        let mut obj_vec = vec![];
        for i in types {
            let idx = self.insert_sym_with_error(const_pool.name_pool[i.0], i.0)?;
            let obj_rc = libstorage.class_table[*i.1].clone();
            let classid = match self.alloc_type_id(idx) {
                Err(_) => return Err(symbol_redefined(i.0)),
                Ok(t) => t,
            };
            obj_vec.push((classid, obj_rc));
        }
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
        let funcs = &stdlib.functions;
        for i in funcs {
            let idx = self.insert_sym_with_error(const_pool.name_pool[i.0], i.0)?;
            // 在将类型全部添加进去之后，需要重新改写函数和类的输入和输出参数
            let mut fobj = i.1.clone();
            self.fix_func(fobj.get_io_mut(), libstorage, const_pool);
            let fobj = Rc::new(fobj);
            self.add_func(idx, fobj.clone());
        }
        // 改写类的重载方法
        Ok(())
    }

    /// import the prelude of stdlib
    ///
    /// # Errors
    ///
    /// This function will return an error if can insert the symbol to scope.
    ///
    /// # Panics
    /// When submodule prelude is not here
    pub fn import_prelude(&mut self, const_pool: &ValuePool) -> Result<(), ErrorInfo> {
        let lib = &stdlib::get_stdlib().sub_modules["prelude"];
        self.import_native_module(lib, stdlib::get_storage(), const_pool)?;
        Ok(())
    }

    pub fn get_module(&self, id: ScopeAllocIdTy) -> Option<Rc<RefCell<SymScope>>> {
        let t = self.imported_modules.get(&id);
        match t {
            None => match self.prev_scope {
                Some(ref prev_scope) => prev_scope.borrow().get_module(id),
                None => None,
            },
            Some(v) => Some(v.clone()),
        }
    }

    pub fn get_function(&self, id: usize) -> Option<Rc<dyn FunctionInterface>> {
        match self.funcs.get(&id) {
            Some(f) => Some(f.clone()),
            None => match self.prev_scope {
                Some(ref prev) => prev.borrow().get_function(id),
                None => None,
            },
        }
    }

    pub fn has_sym(&self, id: usize) -> bool {
        if self.sym_map.contains_key(&id) {
            return true;
        }
        match self.prev_scope {
            Some(ref prev_scope) => prev_scope.borrow().has_sym(id),
            None => false,
        }
    }

    pub fn new_id(&mut self) -> usize {
        let ret = self.scope_sym_id;
        self.scope_sym_id += 1;
        ret
    }

    pub fn insert_sym(&mut self, id: usize) -> Option<usize> {
        let t = self.sym_map.insert(id, self.scope_sym_id);
        self.scope_sym_id += 1;
        // 先前不能存在
        match t {
            None => Some(self.scope_sym_id - 1),
            Some(_) => None,
        }
    }

    /// 返回变量的类型，索引和内存地址
    pub fn get_var(&self, id: ScopeAllocIdTy) -> Option<VarInfo> {
        match self.vars.get(&id) {
            Some(v) => Some(*v),
            None => match self.prev_scope {
                Some(ref prev_scope) => prev_scope.borrow().get_var(id),
                None => None,
            },
        }
    }

    pub fn get_sym(&self, id: usize) -> Option<usize> {
        let t = self.sym_map.get(&id);
        match t {
            None => match self.prev_scope {
                Some(ref prev_scope) => prev_scope.borrow().get_sym(id),
                None => None,
            },
            Some(t) => Some(*t),
        }
    }

    pub fn add_func(&mut self, id: usize, f: Rc<dyn FunctionInterface>) {
        self.funcs.insert(id, f);
    }

    /// 返回变量的索引和内存地址
    pub fn add_var(&mut self, id: ScopeAllocIdTy, ty: TyIdxTy, var_sz: usize) -> (VarIdxTy, usize) {
        let ret_addr = self.var_sz;
        self.vars
            .insert(id, VarInfo::new(ty, self.vars_id, ret_addr));
        let ret = self.vars_id;
        self.vars_id += 1;
        self.var_sz += var_sz;
        (ret, ret_addr)
    }

    pub fn get_var_table_sz(&self) -> usize {
        self.var_sz
    }

    pub fn get_type_id(&self, id: ScopeAllocIdTy) -> Option<ScopeAllocClassId> {
        match self.types.get(&id) {
            None => match self.prev_scope {
                Some(ref prev_scope) => prev_scope.borrow().get_type_id(id),
                None => None,
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

    pub fn get_class_by_class_id(
        &self,
        classid: ScopeAllocClassId,
    ) -> Option<Rc<dyn ClassInterface>> {
        // 不存在的类
        if classid >= self.types_id {
            return None;
        }
        self.types_custom_store.get(&classid).cloned()
    }

    pub fn get_class(&self, classid_idx: ScopeAllocIdTy) -> Option<Rc<dyn ClassInterface>> {
        let t = self.types.get(&classid_idx);
        match t {
            Some(t) => self.get_class_by_class_id(*t),
            None => match self.prev_scope {
                Some(ref prev_scope) => prev_scope.borrow().get_class(classid_idx),
                None => None,
            },
        }
    }

    fn alloc_type_id(&mut self, idx: ScopeAllocIdTy) -> Result<ScopeAllocIdTy, ScopeError> {
        if self.types.insert(idx, self.types_id).is_some() {
            return Err(ScopeError::Redefine);
        }
        let ret = self.types_id;
        self.types_id += 1;
        Ok(ret)
    }

    fn insert_type(
        &mut self,
        id: ScopeAllocClassId,
        f: Rc<dyn ClassInterface>,
    ) -> Result<(), ScopeError> {
        self.types_custom_store.insert(id, f);
        Ok(())
    }

    pub fn add_type(
        &mut self,
        idx: ScopeAllocIdTy,
        obj: Rc<dyn ClassInterface>,
    ) -> Result<ScopeAllocClassId, ScopeError> {
        let ret = self.alloc_type_id(idx)?;
        self.insert_type(ret, obj)?;
        Ok(ret)
    }

    pub fn get_type_id_by_token(&self, ty_name: ConstPoolIndexTy) -> Option<ScopeAllocClassId> {
        let ty_idx_id = self.get_sym(ty_name).unwrap();
        self.get_type_id(ty_idx_id)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_scope() {
        let root_scope = Rc::new(RefCell::new(SymScope::new(None)));
        root_scope.borrow_mut().insert_sym(1);
        let mut son_scope = SymScope::new(Some(root_scope.clone()));
        son_scope.insert_sym(2);
        assert_eq!(son_scope.get_sym(2), Some(1));
        drop(son_scope);
        assert_eq!(root_scope.borrow().get_sym(1), Some(0));
    }
}
