use super::trcbool::TrcBool;
use super::TrcInt;
use super::TrcObj;
use crate::base::codegen::Opcode;
use crate::base::error::*;
use crate::base::stdlib::*;
use crate::compiler::scope::TypeAllowNull;
use crate::compiler::token::TokenType;
use crate::hash_map;
use crate::impl_single_oper;
use crate::tvm::GcMgr;
use crate::{batch_impl_opers, impl_oper};
use derive::trc_class;
use derive::trc_method;
use rust_i18n::t;
use std::collections::hash_map::HashMap;
use std::fmt::Display;

pub type TrcFloatInternal = f64;

#[trc_class]
#[derive(Debug, Clone)]
pub struct TrcFloat {
    pub _value: TrcFloatInternal,
}

impl TrcFloat {
    pub fn new(value: f64) -> TrcFloat {
        Self { _value: value }
    }

    fn override_export() -> HashMap<TokenType, OverrideWrapper> {
        hash_map![
            TokenType::Add => OverrideWrapper::new(Opcode::AddFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info()), false)),
            TokenType::Sub => OverrideWrapper::new(Opcode::SubFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info()), false)),
            TokenType::Mul => OverrideWrapper::new(Opcode::MulFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info()), false)),
            TokenType::Div => OverrideWrapper::new(Opcode::DivFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(Self::export_info()), false)),
            TokenType::ExactDiv => OverrideWrapper::new(Opcode::ExtraDivFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcInt::export_info()), false)),
            TokenType::Equal => OverrideWrapper::new(Opcode::EqFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info()), false)),
            TokenType::NotEqual => OverrideWrapper::new(Opcode::NeFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info()), false)),
            TokenType::Less => OverrideWrapper::new(Opcode::LtFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info()), false)),
            TokenType::LessEqual => OverrideWrapper::new(Opcode::LeFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info()), false)),
            TokenType::Greater => OverrideWrapper::new(Opcode::GtFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info()), false)),
            TokenType::GreaterEqual => OverrideWrapper::new(Opcode::GeFloat, IOType::new(vec![Self::export_info()], TypeAllowNull::Yes(TrcBool::export_info()), false)),
            TokenType::SelfNegative => OverrideWrapper::new(Opcode::SelfNegativeFloat, IOType::new(vec![], TypeAllowNull::Yes(Self::export_info()), false))
        ]
    }
}

pub fn div_float(a: f64, b: f64) -> Result<f64, ErrorInfo> {
    if b == 0.0 {
        return Err(ErrorInfo::new(
            t!(ZERO_DIV, "0" = a),
            t!(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a / b)
}

pub fn exact_div_float(a: f64, b: f64) -> Result<i64, ErrorInfo> {
    if b == 0.0 {
        return Err(ErrorInfo::new(
            t!(ZERO_DIV, "0" = a),
            t!(ZERO_DIVSION_ERROR),
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
