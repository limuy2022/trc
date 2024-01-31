use super::TrcObj;
use crate::impl_single_oper;
use crate::tvm::types::TypeError;
use crate::{base::error::*, batch_impl_opers, impl_oper};
use gettextrs::gettext;
use std::fmt::Display;

pub struct TrcFloat {
    pub value: f64,
}

impl TrcFloat {
    pub fn new(value: f64) -> TrcFloat {
        Self { value }
    }
}

fn div_float(a: f64, b: f64) -> Result<f64, ErrorInfo> {
    if b == 0.0 {
        return Err(ErrorInfo::new(
            gettext!(ZERO_DIV, a),
            gettext(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a / b)
}

impl TrcObj for TrcFloat {
    fn get_type_name(&self) -> &str {
        "float"
    }
    batch_impl_opers! {
        add => +, "+", TrcFloat, TrcFloat,
        sub => -, "-", TrcFloat, TrcFloat,
        mul => *, "*", TrcFloat, TrcFloat
    }
    impl_oper!(div, div_float, "/", TrcFloat, TrcFloat, ?);
    impl_single_oper!(self_negative, -, "-", TrcFloat, TrcFloat);
}

impl Display for TrcFloat {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.value)
    }
}
