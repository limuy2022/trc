use super::super::types::*;
use crate::base::stdlib::*;
use crate::tvm::DynaData;
use crate::tvm::RuntimeResult;
use crate::tvm::TrcFloatInternal;
use derive::{def_module, trc_function};

#[trc_function]
pub fn sin_t(x: float) -> float {
    return f64::sin(x);
}

def_module!(module_name = math, functions = [sin_t => sin], classes = []);
