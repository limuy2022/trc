use super::trcfloat::TrcFloat;
use super::TrcObj;
use crate::base::stdlib::*;
use crate::compiler::token::TokenType;
use crate::hash_map;
use crate::impl_single_oper;
use crate::{base::error::*, batch_impl_opers, impl_oper};
use derive::trc_class;
use derive::trc_method;
use gettextrs::gettext;
use std::collections::hash_map::HashMap;
use std::fmt::Display;

#[trc_class]
#[derive(Debug)]
pub struct TrcInt {
    _value: i64,
}

impl TrcInt {
    pub fn new(value: i64) -> TrcInt {
        TrcInt { _value: value }
    }

    fn override_export() -> HashMap<TokenType, IOType> {
        hash_map![]
    }
}

fn extra_div_int(a: i64, b: i64) -> Result<i64, ErrorInfo> {
    if b == 0 {
        return Err(ErrorInfo::new(
            gettext!(ZERO_DIV, a),
            gettext(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a / b)
}

fn div_int(a: i64, b: i64) -> Result<f64, ErrorInfo> {
    if b == 0 {
        return Err(ErrorInfo::new(
            gettext!(ZERO_DIV, a),
            gettext(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a as f64 / b as f64)
}

fn mod_int(a: i64, b: i64) -> Result<i64, ErrorInfo> {
    if b == 0 {
        return Err(ErrorInfo::new(
            gettext!(ZERO_DIV, a),
            gettext(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a % b)
}

/// won't throw error,although 0^0 is undefined,but to be more convenient to use, so we return 1
fn power_int(a: i64, b: i64) -> i64 {
    if b == 0 {
        return 1;
    }
    let mut t = power_int(a, b / 2);
    t *= t;
    if b % 2 == 1 {
        t *= a;
    }
    t
}

impl Display for TrcInt {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self._value)
    }
}

#[trc_method]
impl TrcObj for TrcInt {
    fn get_type_name(&self) -> &str {
        "int"
    }

    batch_impl_opers!(
        add => +, "+", TrcInt, TrcInt,
        sub => -, "-", TrcInt, TrcInt,
        mul => *, "*", TrcInt, TrcInt,
        bit_and => &, "&", TrcInt, TrcInt,
        bit_or => |, "|", TrcInt, TrcInt,
        bit_left_shift => <<, "<<", TrcInt, TrcInt,
        bit_right_shift => >>, ">>", TrcInt, TrcInt
    );

    impl_oper!(div, div_int, "/", TrcInt, TrcFloat, ?);
    impl_oper!(extra_div, extra_div_int, "//", TrcInt, TrcInt, ?);
    impl_oper!(modd, mod_int, "%", TrcInt, TrcInt, ?);
    impl_oper!(power, power_int, "**", TrcInt, TrcInt,,);
    impl_single_oper!(bit_not, !, "~", TrcInt, TrcInt);
    impl_single_oper!(self_negative, -, "-", TrcInt, TrcInt);
}
