use super::TrcObj;
use crate::libbasic::*;
use crate::{batch_impl_opers, impl_oper, impl_single_oper};
use derive::{trc_class, trc_method};
use rust_i18n::t;
use std::collections::hash_map::HashMap;
use std::fmt::Display;

#[trc_class]
#[derive(Debug, Clone)]
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
            write!(f, "true")
        } else {
            write!(f, "false")
        }
    }
}

impl TrcBool {
    pub fn new(value: bool) -> TrcBool {
        Self { _value: value }
    }

    fn override_export() -> HashMap<OverrideOperations, OverrideWrapper> {
        collection_literals::hash![
            OverrideOperations::And => OverrideWrapper::new(crate::codegen::Opcode::AndBool, IOType::new(vec![Self::export_info()], TypeAllowNull::Some(Self::export_info()), false)),
            OverrideOperations::Or => OverrideWrapper::new(crate::codegen::Opcode::OrBool, IOType::new(vec![Self::export_info()], TypeAllowNull::Some(Self::export_info()), false)),
            OverrideOperations::Not => OverrideWrapper::new(crate::codegen::Opcode::NotBool, IOType::new(vec![Self::export_info()], TypeAllowNull::Some(Self::export_info()), false))
        ]
    }
}
