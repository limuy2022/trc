use lazy_static::lazy_static;
use std::{cell::RefCell, collections::HashMap, fmt::Display, rc::Rc};

use super::{BOOL_ID_POS, FLOAT_ID_POS, INT_ID_POS, STR_ID_POS};

lazy_static! {
    static ref VAR_TYPE: Vec<String> = vec![
        "int".to_string(),
        "float".to_string(),
        "str".to_string(),
        "bool".to_string(),
        "bigint".to_string(),
    ];
}

#[derive(Clone, Debug, PartialEq)]
pub enum TypeAllowNull {
    Yes(Type),
    No,
}

/// Manager of function
#[derive(Clone, Debug, PartialEq)]
pub struct Function {
    args_type: Vec<usize>,
    return_type: TypeAllowNull,
}

impl Function {
    pub fn new(return_type: TypeAllowNull) -> Self {
        Self {
            args_type: vec![],
            return_type,
        }
    }

    pub fn add_argv() {}
}

#[derive(Clone, Debug, PartialEq)]
pub struct Var {
    ty: Function,
}

impl Var {
    pub fn new(ty: Function) -> Self {
        Self { ty }
    }
}

/// Manager of type
#[derive(Clone, Debug, PartialEq)]
pub struct Type {
    attr: Vec<Var>,
    funcs: Vec<Function>,
    pub name: usize,
}

impl Type {
    pub fn new(name: usize) -> Self {
        Self {
            attr: vec![],
            funcs: vec![],
            name,
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
    pub static ref INT_TYPE: Type = Type::new(INT_ID_POS);
    pub static ref FLOAT_TYPE: Type = Type::new(INT_ID_POS);
    pub static ref STR_TYPE: Type = Type::new(INT_ID_POS);
    pub static ref BOOL_TYPE: Type = Type::new(INT_ID_POS);
}

pub struct SymScope {
    prev_scope: Option<Rc<RefCell<SymScope>>>,
    sym_map: HashMap<usize, usize>,
    scope_sym_id: usize,
    types: HashMap<usize, Type>,
    funcs: HashMap<usize, Function>,
    vars: HashMap<usize, Var>,
    native_funcs: HashMap<usize, usize>,
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
            native_funcs: HashMap::new(),
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

    pub fn has_sym(&self, id: usize) -> bool {
        if self.sym_map.contains_key(&id) {
            return true;
        }
        return match self.prev_scope {
            Some(ref prev_scope) => prev_scope.as_ref().borrow().has_sym(id),
            None => false,
        };
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

    pub fn add_func(&mut self, id: usize, f: Function) {
        self.funcs.insert(id, f);
    }

    pub fn add_var(&mut self, id: usize, v: Var) {
        self.vars.insert(id, v);
    }

    pub fn add_type(&mut self, t: Type) {
        self.types.insert(t.name, t);
    }

    pub fn get_type(&self, id: usize) -> Type {
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
