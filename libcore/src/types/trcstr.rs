use super::TrcObj;
use crate::codegen::Opcode;
use crate::error::*;
use crate::gc::GcMgr;
use crate::libbasic::*;
use derive::{trc_class, trc_method};
use rust_i18n::t;
use std::collections::hash_map::HashMap;
use std::fmt::Display;

pub type TrcStrInternal = *mut String;

#[trc_class]
#[derive(Debug, Clone)]
pub struct TrcStr {
    pub _value: TrcStrInternal,
}

fn cat_string(a: &String, b: &String) -> String {
    format!("{a}{b}")
}

#[trc_method]
impl TrcObj for TrcStr {
    fn get_type_name(&self) -> &str {
        "str"
    }

    #[allow(clippy::not_unsafe_ptr_arg_deref)]
    fn add(&self, other: *mut dyn TrcObj, gc: &mut GcMgr) -> ErrorInfoResult<*mut dyn TrcObj> {
        unsafe { self.add_impl(other, gc) }
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

    fn override_export() -> HashMap<OverrideOperations, OverrideWrapper> {
        collection_literals::hash![
            OverrideOperations::Add => OverrideWrapper::new(Opcode::AddStr, IOType::new(vec![Self::export_info()], Some(Self::export_info()), false))
        ]
    }

    unsafe fn add_impl(
        &self,
        other: *mut dyn TrcObj,
        gc: &mut GcMgr,
    ) -> ErrorInfoResult<*mut dyn TrcObj> {
        unsafe {
            match (*other).downcast_ref::<TrcStr>() {
                Some(v) => {
                    let val = gc.alloc(cat_string(&*self._value, &*(v._value)));
                    Ok(gc.alloc(TrcStr::new(val)))
                }
                None => Err(ErrorInfo::new(
                    t!(
                        OPERATOR_IS_NOT_SUPPORT,
                        "0" = "+",
                        "1" = (*other).get_type_name()
                    ),
                    t!(OPERATOR_ERROR),
                )),
            }
        }
    }
}
