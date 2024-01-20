use gettextrs::gettext;
use crate::{base::error::{ErrorInfo, OPERATOR_IS_NOT_SUPPORT, OPERATOR_ERROR}, impl_oper, batch_impl_opers};
use super::{TrcObj, TypeError};

pub struct TrcInt {
    pub value: i64,
}

impl TrcInt {
    pub fn new(value: i64) -> TrcInt {
        TrcInt { value }
    }
}

impl TrcObj for TrcInt {
    fn get_type_name(&self) -> &str {
        "int"
    }

    // impl_oper!(add, +, "+", TrcInt);
    batch_impl_opers!(
        add => +, "+", TrcInt,
        sub => -, "-", TrcInt
    );
}
