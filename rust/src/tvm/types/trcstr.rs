use super::TrcObj;
use crate::base::stdlib::*;
use crate::compiler::token::TokenType;
use crate::hash_map;
use crate::tvm::DynaData;
use crate::{base::error::*, batch_impl_opers, impl_oper};
use derive::{trc_class, trc_function, trc_method};
use gettextrs::gettext;
use std::collections::hash_map::HashMap;
use std::fmt::Display;

#[trc_class]
pub struct TrcStr {
    _value: String,
}

fn cat_string(a: String, b: String) -> String {
    format!("{}{}", a, b)
}

#[trc_method]
impl TrcObj for TrcStr {
    fn get_type_name(&self) -> &str {
        "str"
    }

    batch_impl_opers! {}
    impl_oper!(add, cat_string, "+", TrcStr, TrcStr,,);
}

impl Display for TrcStr {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self._value)
    }
}

impl TrcStr {
    pub fn new(value: String) -> Self {
        Self { _value: value }
    }

    fn override_export() -> HashMap<TokenType, IOType> {
        hash_map![]
    }
}
