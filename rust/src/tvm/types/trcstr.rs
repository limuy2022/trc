use std::collections::hash_map::HashMap;
use std::fmt::Display;

use rust_i18n::t;

use derive::{trc_class, trc_method};

use crate::base::error::*;
use crate::base::stdlib::*;
use crate::compiler::scope::TypeAllowNull;
use crate::compiler::token::TokenType;
use crate::hash_map;
use crate::tvm::GcMgr;

use super::TrcObj;

#[trc_class]
#[derive(Debug, Clone)]
pub struct TrcStr {
    pub _value: *mut String,
}

fn cat_string(a: &String, b: &String) -> String {
    format!("{}{}", a, b)
}

#[trc_method]
impl TrcObj for TrcStr {
    fn get_type_name(&self) -> &str {
        "str"
    }

    fn add(&self, other: *mut dyn TrcObj, gc: &mut GcMgr) -> RuntimeResult<*mut dyn TrcObj> {
        return unsafe {
            match (*other).downcast_ref::<TrcStr>() {
                Some(v) => {
                    let val = gc.alloc(cat_string(&*self._value, &*((*v)._value)));
                    Ok(gc.alloc(TrcStr::new(val)))
                }
                None => {
                    Err(ErrorInfo::new(
                        t!(
                            OPERATOR_IS_NOT_SUPPORT,
                            "0" = "+",
                            "1" = (*other).get_type_name()
                        ),
                        t!(OPERATOR_ERROR),
                    ))
                }
            }
        }
    }
}

impl Display for TrcStr {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", unsafe { &*self._value })
    }
}

impl TrcStr {
    pub fn new(value: *mut String) -> Self {
        Self { _value: value }
    }

    fn override_export() -> HashMap<TokenType, OverrideWrapper> {
        hash_map![
            TokenType::Add => OverrideWrapper::new(crate::base::codegen::Opcode::AddStr, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info()), false))
        ]
    }
}
