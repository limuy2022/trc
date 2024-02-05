use super::TrcObj;
use crate::base::stdlib::*;
use crate::compiler::token::TokenType;
use crate::hash_map;
use derive::{trc_class, trc_method};
use std::collections::hash_map::HashMap;
use std::fmt::Display;

#[trc_class]
#[derive(Debug)]
pub struct TrcChar {
    pub _value: char,
}

#[trc_method]
impl TrcObj for TrcChar {
    fn get_type_name(&self) -> &str {
        "char"
    }
}

impl Display for TrcChar {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self._value)
    }
}

impl TrcChar {
    pub fn new(value: char) -> TrcChar {
        Self { _value: value }
    }

    fn override_export() -> HashMap<TokenType, IOType> {
        hash_map![]
    }
}
