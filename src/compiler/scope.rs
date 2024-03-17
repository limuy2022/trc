use super::{
    token::{ConstPoolIndexTy, Token},
    ValuePool,
};
use crate::base::stdlib::{
    get_stdclass_end, get_stdlib, ArgsNameTy, ClassInterface, FunctionInterface, IOType,
    OverrideWrapper, Stdlib, STD_CLASS_TABLE,
};
use std::{cell::RefCell, collections::HashMap, fmt::Display, rc::Rc, usize};

pub type ScopeAllocIdTy = usize;
#[derive(Clone, Debug)]
pub enum TypeAllowNull {
    Yes(ScopeAllocIdTy),
    No,
}

impl TypeAllowNull {
    pub fn unwrap(&self) -> usize {
        match self {
            TypeAllowNull::Yes(t) => *t,
            TypeAllowNull::No => panic!("null"),
        }
    }
}

impl Display for TypeAllowNull {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            TypeAllowNull::Yes(t) => write!(f, "{}", t),
            TypeAllowNull::No => write!(f, "null"),
        }
    }
}

/// Manager of function
#[derive(Clone, Debug)]
pub struct CustomFunction {
    io: IOType,
    args_names: ArgsNameTy,
    pub custom_id: FuncIdxTy,
    name: String,
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
}

pub type Type = Box<dyn ClassInterface>;
pub type Func = Box<dyn FunctionInterface>;

pub type TyIdxTy = ScopeAllocIdTy;

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

    fn get_override_func(&self, oper_token: super::token::TokenType) -> Option<&OverrideWrapper> {
        None
    }
}

pub type ScopeAllocClassId = usize;
pub type VarIdxTy = ScopeAllocIdTy;
pub type FuncIdxTy = ScopeAllocIdTy;
pub type FuncBodyTy = Vec<(Token, usize)>;

#[derive(Default)]
pub struct SymScope {
    // 父作用域
    prev_scope: Option<Rc<RefCell<SymScope>>>,
    // 管理符号之间的映射,由token在name pool中的id映射到符号表中的id
    sym_map: HashMap<ConstPoolIndexTy, ScopeAllocIdTy>,
    // 当前作用域要分配的下一个ID,也就是当前作用域的最大id+1
    scope_sym_id: ScopeAllocIdTy,
    // ID到class id的映射
    types: HashMap<ScopeAllocIdTy, ScopeAllocClassId>,
    // ID到函数的映射
    funcs: HashMap<ScopeAllocIdTy, Box<dyn FunctionInterface>>,
    // id到变量类型的映射
    vars: HashMap<ScopeAllocIdTy, (TyIdxTy, VarIdxTy)>,
    // 由token id到模块的映射
    modules: HashMap<ScopeAllocIdTy, &'static Stdlib>,
    // 当前作用域可以分配的下一个class id
    types_id: ScopeAllocClassId,
    vars_id: ScopeAllocIdTy,
    funcs_custom_id: ScopeAllocIdTy,
    // 用户自定义的类型储存位置
    types_custom_store: HashMap<ScopeAllocClassId, CustomType>,
    // 作用域暂时储存的函数token
    pub funcs_temp_store: Vec<(FuncIdxTy, Vec<(Token, usize)>)>,
}

impl SymScope {
    pub fn new(prev_scope: Option<Rc<RefCell<SymScope>>>) -> Self {
        let mut ret = Self {
            prev_scope: prev_scope.clone(),
            ..Default::default()
        };
        match prev_scope {
            Some(prev_scope) => {
                ret.scope_sym_id = prev_scope.as_ref().borrow().scope_sym_id;
                ret.types_id = prev_scope.as_ref().borrow().types_id;
                ret.vars_id = prev_scope.as_ref().borrow().vars_id;
                ret.funcs_custom_id = prev_scope.as_ref().borrow().funcs_custom_id;
            }
            None => {
                ret.types_id = get_stdclass_end();
                ret.vars_id = 0;
                ret.funcs_custom_id = 0;
            }
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
        self.funcs_temp_store.push((ret, body));
        f.custom_id = ret;
        self.add_func(id, Box::new(f));
        self.funcs_custom_id += 1;
        ret
    }

    /// import the module defined in rust
    pub fn import_native_module(&mut self, id: ScopeAllocIdTy, stdlib: &'static Stdlib) {
        self.modules.insert(id, stdlib);
    }

    pub fn import_prelude(&mut self, const_pool: &ValuePool) {
        let funcs = &get_stdlib().sub_modules.get("prelude").unwrap().functions;
        for i in funcs {
            let idx = self.insert_sym(const_pool.name_pool[i.0]).unwrap();
            self.add_func(idx, Box::new(i.1.clone()));
        }
        let types = &get_stdlib().sub_modules.get("prelude").unwrap().classes;
        for i in types {
            let idx = self.insert_sym(const_pool.name_pool[i.0]).unwrap();
            self.add_type(idx, *i.1);
        }
    }

    pub fn get_module(&mut self, id: usize) {}

    pub fn get_function(&self, id: usize) -> Option<Box<dyn FunctionInterface>> {
        match self.funcs.get(&id) {
            Some(f) => Some(f.clone()),
            None => match self.prev_scope {
                Some(ref prev) => prev.as_ref().borrow().get_function(id),
                None => None,
            },
        }
    }

    pub fn has_sym(&self, id: usize) -> bool {
        if self.sym_map.contains_key(&id) {
            return true;
        }
        match self.prev_scope {
            Some(ref prev_scope) => prev_scope.as_ref().borrow().has_sym(id),
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
        match t {
            None => Some(self.scope_sym_id - 1),
            Some(_) => None,
        }
    }

    pub fn get_var(&self, id: ScopeAllocIdTy) -> Option<(TyIdxTy, VarIdxTy)> {
        match self.vars.get(&id) {
            Some(v) => Some(*v),
            None => match self.prev_scope {
                Some(ref prev_scope) => prev_scope.as_ref().borrow().get_var(id),
                None => None,
            },
        }
    }

    pub fn get_sym(&self, id: usize) -> Option<usize> {
        let t = self.sym_map.get(&id);
        match t {
            None => match self.prev_scope {
                Some(ref prev_scope) => prev_scope.as_ref().borrow().get_sym(id),
                None => None,
            },
            Some(t) => Some(*t),
        }
    }

    pub fn add_func(&mut self, id: usize, f: Box<dyn FunctionInterface>) {
        self.funcs.insert(id, f);
    }

    pub fn add_var(&mut self, id: ScopeAllocIdTy, ty: TyIdxTy) -> VarIdxTy {
        self.vars.insert(id, (ty, self.vars_id));
        let ret = self.vars_id;
        self.vars_id += 1;
        ret
    }

    pub fn add_type(&mut self, id: usize, t: usize) {
        self.types.insert(id, t);
    }

    pub fn get_type(&self, id: usize) -> Option<usize> {
        match self.types.get(&id) {
            None => match self.prev_scope {
                Some(ref prev_scope) => prev_scope.as_ref().borrow().get_type(id),
                None => None,
            },
            Some(t) => Some(*t),
        }
    }

    pub fn get_scope_last_idx(&self) -> usize {
        self.scope_sym_id
    }

    pub fn get_var_table_sz(&self) -> usize {
        self.vars_id
    }

    pub fn get_class(&self, classid: usize) -> Option<Type> {
        // 在标准库界限内
        if classid < get_stdclass_end() {
            unsafe {
                return Some(Box::new(STD_CLASS_TABLE[classid].clone()));
            }
        }
        // 不存在的类
        if classid >= self.types_id {
            return None;
        }
        let t = self.types.get(&classid);
        match t {
            Some(t) => Some(Box::new(self.types_custom_store.get(t).unwrap().clone())),
            None => match self.prev_scope {
                Some(ref prev_scope) => prev_scope.as_ref().borrow().get_class(classid),
                None => None,
            },
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_scope() {
        let root_scope = Rc::new(RefCell::new(SymScope::new(None)));
        root_scope.as_ref().borrow_mut().insert_sym(1);
        let mut son_scope = SymScope::new(Some(root_scope.clone()));
        son_scope.insert_sym(2);
        assert_eq!(son_scope.get_sym(2), Some(1));
        drop(son_scope);
        assert_eq!(root_scope.as_ref().borrow().get_sym(1), Some(0));
    }
}
