use super::TrcObj;
use crate::{base::error::*, batch_impl_opers, impl_oper, tvm::types::TypeError};
use gettextrs::gettext;
use std::fmt::Display;

pub struct TrcStr {
    value: String,
}

fn cat_string(a: String, b: String) -> String {
    format!("{}{}", a, b)
}

impl TrcObj for TrcStr {
    fn get_type_name(&self) -> &str {
        "str"
    }

    batch_impl_opers! {}
    impl_oper!(add, cat_string, "+", TrcStr, TrcStr,,);
}

impl Display for TrcStr {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.value)
    }
}

impl TrcStr {
    pub fn new(value: String) -> Self {
        Self { value }
    }
}
