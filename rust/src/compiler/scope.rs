use lazy_static::lazy_static;
use std::{cell::RefCell, collections::HashMap, rc::Rc};

lazy_static! {
    static ref VAR_TYPE: Vec<String> = vec![
        "int".to_string(),
        "float".to_string(),
        "str".to_string(),
        "bool".to_string(),
        "bigint".to_string(),
    ];
}

pub struct SymScope {
    prev_scope: Option<Rc<RefCell<SymScope>>>,
    sym_map: HashMap<usize, usize>,
    scope_sym_id: usize,
}

impl SymScope {
    pub fn new(prev_scope: Option<Rc<RefCell<SymScope>>>) -> Self {
        let mut ret = Self {
            prev_scope: prev_scope.clone(),
            sym_map: HashMap::new(),
            scope_sym_id: 0,
        };
        match prev_scope {
            Some(prev_scope) => {
                ret.scope_sym_id = prev_scope.as_ref().borrow().scope_sym_id;
            }
            None => {}
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
