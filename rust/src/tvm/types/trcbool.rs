use super::{TrcObj, TypeError};
use crate::base::error::{ErrorInfo, OPERATOR_ERROR, OPERATOR_IS_NOT_SUPPORT};
use crate::{batch_impl_opers, impl_oper, impl_single_oper};
use gettextrs::gettext;
use std::fmt::Display;

pub struct TrcBool {
    pub value: bool,
}

impl TrcObj for TrcBool {
    fn get_type_name(&self) -> &str {
        "bool"
    }

    impl_single_oper!(not, !, "not", TrcBool, TrcBool);
    batch_impl_opers!(
        and => &&, "and", TrcBool, TrcBool,
        or => ||, "or", TrcBool, TrcBool
    );
}

impl Display for TrcBool {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        if self.value {
            write!(f, "True")
        } else {
            write!(f, "False")
        }
    }
}

impl TrcBool {
    pub fn new(value: bool) -> TrcBool {
        Self { value }
    }
}
