use super::TrcObj;
use crate::base::stdlib::*;
use crate::compiler::scope::TypeAllowNull;
use crate::compiler::token::TokenType;
use crate::hash_map;
use crate::{base::error::*, batch_impl_opers, impl_oper};
use derive::{trc_class, trc_method};
use gettextrs::gettext;
use std::collections::hash_map::HashMap;
use std::fmt::Display;

#[trc_class]
#[derive(Debug)]
pub struct TrcStr {
    pub _value: String,
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

    fn override_export() -> HashMap<TokenType, OverrideWrapper> {
        hash_map![
            TokenType::Add => OverrideWrapper::new(crate::base::codegen::Opcode::AddStr, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info()), false))
        ]
    }
}
