use super::trcbool::TrcBool;
use super::trcfloat::TrcFloat;
use super::TrcObj;
use crate::codegen::Opcode;
use crate::impl_single_oper;
use crate::libbasic::*;
use crate::{batch_impl_opers, error::*, impl_oper};
use derive::trc_class;
use derive::trc_method;
use rust_i18n::t;
use std::collections::hash_map::HashMap;
use std::fmt::Display;

pub type TrcIntInternal = i64;

#[trc_class]
#[derive(Debug, Clone)]
pub struct TrcInt {
    pub _value: TrcIntInternal,
}

impl TrcInt {
    pub fn new(value: TrcIntInternal) -> TrcInt {
        TrcInt { _value: value }
    }

    fn override_export() -> HashMap<OverrideOperations, OverrideWrapper> {
        collection_literals::hash![OverrideOperations::Add => OverrideWrapper::new(Opcode::AddInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::Sub => OverrideWrapper::new(Opcode::SubInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::Mul => OverrideWrapper::new(Opcode::MulInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::Div => OverrideWrapper::new(Opcode::DivInt,IOType::new(vec![Self::export_info()], Some(TrcFloat::export_info()), false)),
        OverrideOperations::Mod => OverrideWrapper::new(Opcode::ModInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::Power => OverrideWrapper::new(Opcode::PowerInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::BitAnd => OverrideWrapper::new(Opcode::BitAndInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::BitOr => OverrideWrapper::new(Opcode::BitOrInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::Xor => OverrideWrapper::new(Opcode::XorInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::ExactDiv => OverrideWrapper::new(Opcode::ExactDivInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::BitLeftShift => OverrideWrapper::new(Opcode::BitLeftShiftInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::BitRightShift => OverrideWrapper::new(Opcode::BitRightShiftInt,IOType::new(vec![Self::export_info()], Some(Self::export_info()), false)),
        OverrideOperations::BitNot => OverrideWrapper::new(Opcode::BitNotInt,IOType::new(vec![], Some(Self::export_info()), false)),
        OverrideOperations::SelfNegative => OverrideWrapper::new(Opcode::SelfNegativeInt,IOType::new(vec![], Some(Self::export_info()), false)),
        OverrideOperations::Equal => OverrideWrapper::new(Opcode::EqInt,IOType::new(vec![Self::export_info()], Some(TrcBool::export_info()), false)),
        OverrideOperations::NotEqual => OverrideWrapper::new(Opcode::NeInt,IOType::new(vec![Self::export_info()], Some(TrcBool::export_info()), false)),
        OverrideOperations::Less => OverrideWrapper::new(Opcode::LtInt,IOType::new(vec![Self::export_info()], Some(TrcBool::export_info()), false)),
        OverrideOperations::LessEqual => OverrideWrapper::new(Opcode::LeInt,IOType::new(vec![Self::export_info()], Some(TrcBool::export_info()), false)),
        OverrideOperations::Greater => OverrideWrapper::new(Opcode::GtInt,IOType::new(vec![Self::export_info()], Some(TrcBool::export_info()), false)),
        OverrideOperations::GreaterEqual => OverrideWrapper::new(Opcode::GeInt,IOType::new(vec![Self::export_info()], Some(TrcBool::export_info()), false))
        ]
    }
}

pub fn exact_div_int(a: TrcIntInternal, b: TrcIntInternal) -> Result<TrcIntInternal, ErrorInfo> {
    if b == 0 {
        return Err(ErrorInfo::new(
            t!(ZERO_DIV, "0" = a),
            t!(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a / b)
}

pub fn div_int(a: i64, b: i64) -> Result<f64, ErrorInfo> {
    if b == 0 {
        return Err(ErrorInfo::new(
            t!(ZERO_DIV, "0" = a),
            t!(ZERO_DIVSION_ERROR),
        ));
    }
    Ok(a as f64 / b as f64)
}

pub fn mod_int(a: i64, b: i64) -> Result<i64, ErrorInfo> {
    if b == 0 {
        return Err(ErrorInfo::new(
            t!(ZERO_DIV, "0" = a),
            t!(ZERO_DIVSION_ERROR),
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
