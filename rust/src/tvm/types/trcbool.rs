use super::TrcObj;
use crate::base::error::*;
use crate::base::error::{ErrorInfo, OPERATOR_ERROR, OPERATOR_IS_NOT_SUPPORT};
use crate::base::stdlib::*;
use crate::compiler::scope::TypeAllowNull;
use crate::compiler::token::TokenType;
use crate::{batch_impl_opers, hash_map, impl_oper, impl_single_oper};
use derive::{trc_class, trc_method};
use gettextrs::gettext;
use std::collections::hash_map::HashMap;
use std::fmt::Display;

#[trc_class]
#[derive(Debug)]
pub struct TrcBool {
    pub _value: bool,
}

#[trc_method]
impl TrcObj for TrcBool {
    fn get_type_name(&self) -> &str {
        "bool"
    }

    impl_single_oper!(not, !, "!", TrcBool, TrcBool);
    batch_impl_opers!(
        and => &&, "&&", TrcBool, TrcBool,
        or => ||, "||", TrcBool, TrcBool
    );
}

impl Display for TrcBool {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        if self._value {
            write!(f, "True")
        } else {
            write!(f, "False")
        }
    }
}

impl TrcBool {
    pub fn new(value: bool) -> TrcBool {
        Self { _value: value }
    }

    fn override_export() -> HashMap<TokenType, IOType> {
        hash_map![
            TokenType::And => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
            TokenType::Or => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
            TokenType::Not => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info()))
        ]
    }
}
