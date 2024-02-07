use super::trcbool::TrcBool;
use super::trcfloat::TrcFloat;
use super::TrcObj;
use crate::base::stdlib::*;
use crate::compiler::scope::TypeAllowNull;
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
    pub _value: i64,
}

impl TrcInt {
    pub fn new(value: i64) -> TrcInt {
        TrcInt { _value: value }
    }

    fn override_export() -> HashMap<TokenType, IOType> {
        hash_map![TokenType::Add => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::Sub => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::Mul => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::Div => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcFloat::export_info())),
        TokenType::Mod => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::Power => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::BitAnd => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::BitOr => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::Xor => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::ExactDiv => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::BitLeftShift => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::BitRightShift => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
        TokenType::BitNot => IOType::new(vec![], TypeAllowNull::Yes(Self::export_info())),
        TokenType::SelfNegative => IOType::new(vec![], TypeAllowNull::Yes(Self::export_info())),
        TokenType::Equal => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
        TokenType::NotEqual => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
        TokenType::Less => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
        TokenType::LessEqual => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
        TokenType::Greater => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
        TokenType::GreaterEqual => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info()))
        ]
    }
}

pub fn exact_div_int(a: i64, b: i64) -> Result<i64, ErrorInfo> {
    if b == 0 {
        return Err(ErrorInfo::new(
            gettext!(ZERO_DIV, a),
            gettext(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a / b)
}

pub fn div_int(a: i64, b: i64) -> Result<f64, ErrorInfo> {
    if b == 0 {
        return Err(ErrorInfo::new(
            gettext!(ZERO_DIV, a),
            gettext(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a as f64 / b as f64)
}

pub fn mod_int(a: i64, b: i64) -> Result<i64, ErrorInfo> {
    if b == 0 {
        return Err(ErrorInfo::new(
            gettext!(ZERO_DIV, a),
            gettext(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a % b)
}

/// won't throw error,although 0^0 is undefined,but to be more convenient to use, so we return 1
pub fn power_int(a: i64, b: i64) -> i64 {
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
        bit_right_shift => >>, ">>", TrcInt, TrcInt,
        xor => ^, "^", TrcInt, TrcInt,
        gt => >, ">", TrcInt, TrcBool,
        lt => <, "<", TrcInt, TrcBool,
        eq => ==, "==", TrcInt, TrcBool,
        ne => !=, "!=",TrcInt, TrcBool,
        ge => >=, ">=",TrcInt, TrcBool,
        le => <=, "<=",TrcInt, TrcBool
    );

    impl_oper!(div, div_int, "/", TrcInt, TrcFloat, ?);
    impl_oper!(extra_div, exact_div_int, "//", TrcInt, TrcInt, ?);
    impl_oper!(modd, mod_int, "%", TrcInt, TrcInt, ?);
    impl_oper!(power, power_int, "**", TrcInt, TrcInt,,);
    impl_single_oper!(bit_not, !, "~", TrcInt, TrcInt);
    impl_single_oper!(self_negative, -, "-", TrcInt, TrcInt);
}
