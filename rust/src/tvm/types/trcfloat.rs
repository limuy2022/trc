use super::trcbool::TrcBool;
use super::TrcInt;
use super::TrcObj;
use crate::base::error::*;
use crate::base::stdlib::*;
use crate::compiler::scope::TypeAllowNull;
use crate::compiler::token::TokenType;
use crate::hash_map;
use crate::impl_single_oper;
use crate::{batch_impl_opers, impl_oper};
use derive::trc_class;
use derive::trc_method;
use gettextrs::gettext;
use std::collections::hash_map::HashMap;
use std::fmt::Display;

#[trc_class]
#[derive(Debug)]
pub struct TrcFloat {
    pub _value: f64,
}

impl TrcFloat {
    pub fn new(value: f64) -> TrcFloat {
        Self { _value: value }
    }

    fn override_export() -> HashMap<TokenType, IOType> {
        hash_map![
            TokenType::Add => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
            TokenType::Sub => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
            TokenType::Mul => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
            TokenType::Div => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info())),
            TokenType::ExactDiv => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcInt::export_info())),
            TokenType::Equal => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
            TokenType::NotEqual => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
            TokenType::Less => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
            TokenType::LessEqual => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
            TokenType::Greater => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
            TokenType::GreaterEqual => IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info())),
            TokenType::SelfNegative => IOType::new(vec![], TypeAllowNull::Yes(Self::export_info()))
        ]
    }
}

pub fn div_float(a: f64, b: f64) -> Result<f64, ErrorInfo> {
    if b == 0.0 {
        return Err(ErrorInfo::new(
            gettext!(ZERO_DIV, a),
            gettext(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a / b)
}

pub fn exact_div_float(a: f64, b: f64) -> Result<i64, ErrorInfo> {
    if b == 0.0 {
        return Err(ErrorInfo::new(
            gettext!(ZERO_DIV, a),
            gettext(ZERO_DIVSION_ERROR),
        ));
    }
    Ok((a / b).floor() as i64)
}

#[trc_method]
impl TrcObj for TrcFloat {
    fn get_type_name(&self) -> &str {
        "float"
    }
    batch_impl_opers! {
        add => +, "+", TrcFloat, TrcFloat,
        sub => -, "-", TrcFloat, TrcFloat,
        mul => *, "*", TrcFloat, TrcFloat,
        gt => >, ">", TrcFloat, TrcBool,
        lt => <, "<", TrcFloat, TrcBool,
        eq => ==, "==", TrcFloat, TrcBool,
        ne => !=, "!=",TrcFloat, TrcBool,
        ge => >=, ">=",TrcFloat, TrcBool,
        le => <=, "<=",TrcFloat, TrcBool
    }
    impl_oper!(div, div_float, "/", TrcFloat, TrcFloat, ?);
    impl_oper!(extra_div, exact_div_float, "//", TrcFloat, TrcInt, ?);
    impl_single_oper!(self_negative, -, "-", TrcFloat, TrcFloat);
}

impl Display for TrcFloat {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self._value)
    }
}
