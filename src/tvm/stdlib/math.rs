use derive::trc_const;
use derive::{def_module, trc_function};

#[trc_function]
pub fn sin(x: float) -> float {
    return f64::sin(x);
}

#[trc_function]
pub fn tan(x: float) -> float {
    return f64::tan(x);
}

#[trc_function]
pub fn cos(x: float) -> float {
    return f64::cos(x);
}

#[trc_function]
pub fn atan(x: float) -> float {
    return f64::atan(x);
}

#[trc_function]
pub fn asin(x: float) -> float {
    return f64::asin(x);
}

#[trc_function]
pub fn acos(x: float) -> float {
    return f64::acos(x);
}

#[trc_const]
const PI: f64 = 3.14159265;

def_module!(module_name = math, functions = [sin => sin, tan => tan, cos => cos, atan => atan, asin => asin, acos => acos], classes = [], consts = [PI]);
