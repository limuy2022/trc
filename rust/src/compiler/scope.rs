use super::{ValuePool, BOOL_ID_POS, FLOAT_ID_POS, INT_ID_POS, STR_ID_POS};
use crate::base::error::*;
use crate::base::stdlib::{FunctionInterface, StdlibNode, STDLIB_LIST};
use lazy_static::lazy_static;
use std::{cell::RefCell, collections::HashMap, fmt::Display, rc::Rc};

lazy_static! {
    static ref VAR_TYPE: Vec<String> = vec![
        "int".to_string(),
        "float".to_string(),
        "str".to_string(),
        "bool".to_string(),
        "bigint".to_string(),
    ];
}

#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub enum TypeAllowNull {
    Yes(Type),
    No,
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
#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub struct Function {
    args_type: Vec<Type>,
    return_type: TypeAllowNull,
    start_address: usize,
}

impl Function {
    pub fn new(return_type: TypeAllowNull, start_address: usize) -> Self {
        Self {
            args_type: vec![],
            return_type,
            start_address,
        }
    }

    pub fn add_argv() {}
}

impl FunctionInterface for Function {
    fn check_argvs(&self, argvs: Vec<Type>) -> Result<(), crate::base::error::ErrorInfo> {
        if argvs.len() != self.args_type.len() {
            return Err(ErrorInfo::new(
                gettextrs::gettext!(ARGU_NUMBER, self.args_type.len(), argvs.len()),
                gettextrs::gettext(ARGUMENT_ERROR),
            ));
        }
        for i in 0..self.args_type.len() {
            if argvs[i] != self.args_type[i] && self.args_type[i] != Type::Any {
                return Err(ErrorInfo::new(
                    gettextrs::gettext!(EXPECT_TYPE, self.args_type[i], argvs[i]),
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

#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub struct Var {
    ty: Type,
}

impl Var {
    pub fn new(ty: Type) -> Self {
        Self { ty }
    }
}

#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub enum Type {
    Common(CommonType),
    Any,
}

impl Display for Type {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Type::Common(t) => write!(f, "{}", t),
            Type::Any => write!(f, "any"),
        }
    }
}

/// Manager of type
#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub struct CommonType {
    attr: Vec<Var>,
    funcs: Vec<Function>,
    pub name: usize,
    pub origin_name: String,
}

impl Display for CommonType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.origin_name)
    }
}

impl CommonType {
    pub fn new(name: usize, origin_name: String) -> Self {
        Self {
            attr: vec![],
            funcs: vec![],
            name,
            origin_name,
        }
    }

    pub fn add_attr(&mut self, attr: Var) {
        self.attr.push(attr);
    }

    pub fn add_func(&mut self, f: Function) {
        self.funcs.push(f);
    }
}
lazy_static! {
    pub static ref INT_TYPE: Type = Type::Common(CommonType::new(INT_ID_POS, "int".to_string()));
    pub static ref FLOAT_TYPE: Type =
        Type::Common(CommonType::new(FLOAT_ID_POS, "float".to_string()));
    pub static ref STR_TYPE: Type = Type::Common(CommonType::new(STR_ID_POS, "str".to_string()));
    pub static ref BOOL_TYPE: Type = Type::Common(CommonType::new(BOOL_ID_POS, "bool".to_string()));
}

pub struct SymScope {
    prev_scope: Option<Rc<RefCell<SymScope>>>,
    sym_map: HashMap<usize, usize>,
    scope_sym_id: usize,
    types: HashMap<usize, CommonType>,
    funcs: HashMap<usize, Box<dyn FunctionInterface>>,
    vars: HashMap<usize, Var>,
    modules: HashMap<usize, &'static StdlibNode>,
}

impl SymScope {
    pub fn new(prev_scope: Option<Rc<RefCell<SymScope>>>) -> Self {
        let mut ret = Self {
            prev_scope: prev_scope.clone(),
            sym_map: HashMap::new(),
            scope_sym_id: 0,
            types: HashMap::new(),
            funcs: HashMap::new(),
            vars: HashMap::new(),
            modules: HashMap::new(),
        };
        match prev_scope {
            Some(prev_scope) => {
                ret.scope_sym_id = prev_scope.as_ref().borrow().scope_sym_id;
            }
            None => {
                ret.add_type(INT_TYPE.clone());
                ret.add_type(FLOAT_TYPE.clone());
                ret.add_type(BOOL_TYPE.clone());
                ret.add_type(STR_TYPE.clone());
            }
        }
        ret
    }

    /// import the module defined in rust
    pub fn import_native_module(&mut self, id: usize, stdlib: &'static StdlibNode) {
        self.modules.insert(id, stdlib);
    }

    pub fn import_prelude(&mut self, const_pool: &ValuePool) {
        for i in &STDLIB_LIST.sons.get("prelude").unwrap().functions {
            self.add_func(const_pool.name_pool[&i.name], Box::new((*i).clone()));
        }
    }

    pub fn has_sym(&self, id: usize) -> bool {
        if self.sym_map.contains_key(&id) {
            return true;
        }
        return match self.prev_scope {
            Some(ref prev_scope) => prev_scope.as_ref().borrow().has_sym(id),
            None => false,
        };
    }

    pub fn new_id(&mut self) -> usize {
        let ret = self.scope_sym_id;
        self.scope_sym_id += 1;
        return ret;
    }

    pub fn insert_sym(&mut self, id: usize) -> usize {
        let t = self.sym_map.entry(id).or_insert(self.scope_sym_id);
        if *t == self.scope_sym_id {
            self.scope_sym_id += 1;
        }
        return *t;
    }

    pub fn get_sym_idx(&self, id: usize) -> Option<usize> {
        let t = self.sym_map.get(&id);
        match t {
            None => {
                return match self.prev_scope {
                    Some(ref prev_scope) => prev_scope.as_ref().borrow().get_sym_idx(id),
                    None => None,
                }
            }
            Some(t) => {
                return Some(*t);
            }
        }
    }

    pub fn add_func(&mut self, id: usize, f: Box<dyn FunctionInterface>) {
        self.funcs.insert(id, f);
    }

    pub fn add_var(&mut self, id: usize, v: Var) {
        self.vars.insert(id, v);
    }

    pub fn add_type(&mut self, t: Type) {
        match t {
            Type::Common(t) => {
                self.types.insert(t.name, t);
            }
            _ => {}
        }
    }

    pub fn get_type(&self, id: usize) -> CommonType {
        return self.types.get(&id).unwrap().clone();
    }

    pub fn get_scope_last_idx(&self) -> usize {
        self.scope_sym_id
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
        assert_eq!(son_scope.get_sym_idx(2), Some(1));
        drop(son_scope);
        assert_eq!(root_scope.as_ref().borrow().get_sym_idx(1), Some(0));
    }
}
