use crate::base::error::*;
use crate::tvm::DynaData;
use downcast_rs::{impl_downcast, Downcast};
use rust_i18n::t;
use std::fmt::Debug;

pub mod data_structure;
pub mod trcbigint;
pub mod trcbool;
pub mod trcchar;
pub mod trcfloat;
pub mod trcint;
pub mod trcstr;

pub use trcbool::TrcBool;
pub use trcchar::TrcChar;
pub use trcfloat::TrcFloat;
pub use trcint::TrcInt;
pub use trcstr::TrcStr;

/// help to generate the same error reporter functions
macro_rules! batch_unsupported_operators {
    ($($traie_name:ident => $oper_name:expr),*) => {
        $(
        fn $traie_name(&self, _:Box<dyn TrcObj>) -> RuntimeResult<Box<dyn TrcObj>> {
            return Err(ErrorInfo::new(
                t!(
                    OPERATOR_IS_NOT_SUPPORT,
                    "0"=$oper_name,
                    "1"=self.get_type_name()
                ),
                t!(SYNTAX_ERROR),
            ));
        })*
    };
}

#[macro_export]
macro_rules! impl_oper {
    // for unsupported operator in rust
    ($trait_oper_fn_name:ident, $oper:ident, $error_oper_name:expr, $self_type:ident, $newtype:ident, $whether_throw_error:tt) => {
        fn $trait_oper_fn_name(&self, other:Box<dyn TrcObj>) -> RuntimeResult<Box<dyn TrcObj>> {
            match other.downcast_ref::<$self_type>() {
                Some(v) => {
                    return Ok(Box::new($newtype::new($oper(self._value.clone(), v._value.clone())$whether_throw_error)));
                },
                None => {
                    return Err(ErrorInfo::new(t!(OPERATOR_IS_NOT_SUPPORT, "0"=$error_oper_name, "1"=other.get_type_name()), t!(OPERATOR_ERROR)))
                }
            }
        }
    };
    // for supported operator in rust
    ($trait_oper_fn_name:ident, $oper:tt, $error_oper_name:expr, $self_type:ident, $newtype:ident) => {
        fn $trait_oper_fn_name(&self, other:Box<dyn TrcObj>) -> RuntimeResult<Box<dyn TrcObj>> {
            match other.downcast_ref::<$self_type>() {
                Some(v) => {
                    return Ok(Box::new($newtype::new(self._value $oper v._value)));
                },
                None => {
                    return Err(ErrorInfo::new(t!(OPERATOR_IS_NOT_SUPPORT, "0"=$error_oper_name, "1"=other.get_type_name()), t!(OPERATOR_ERROR)))
                }
            }
        }
    };
}

#[macro_export]
macro_rules! batch_impl_opers {
    ($($trait_oper_fn_name:ident => $oper:tt, $error_oper_name:expr, $self_type:ident, $newtype:ident),*) => {
        $(
            impl_oper!($trait_oper_fn_name, $oper, $error_oper_name, $self_type, $newtype);
        )*
    };
}

#[macro_export]
macro_rules! impl_single_oper {
    ($trait_oper_fn_name:ident, $oper:tt, $error_oper_name:expr, $self_type:ident, $newtype:ident) => {
        fn $trait_oper_fn_name(&self) -> RuntimeResult<Box<dyn TrcObj>> {
            return Ok(Box::new($newtype::new($oper self._value)));
        }
    };
}

macro_rules! gen_interface {
    ($funcname:ident, 2) => {
        pub fn $funcname(dydata: &mut DynaData) -> RuntimeResult<()> {
            DynaData::check_stack(&mut dydata.obj_stack, 2)?;
            let t2 = dydata.obj_stack.pop().unwrap();
            let t1 = dydata.obj_stack.pop().unwrap();
            dydata.obj_stack.push(t1.$funcname(t2)?);
            Ok(())
        }
    };
    ($funcname:ident, 1) => {
        pub fn $funcname(dydata: &mut DynaData) -> RuntimeResult<()> {
            DynaData::check_stack(&mut dydata.obj_stack, 1)?;
            let t1 = dydata.obj_stack.pop().unwrap();
            dydata.obj_stack.push(t1.$funcname()?);
            Ok(())
        }
    };
}

pub trait TrcObj: Downcast + std::fmt::Display + Debug {
    batch_unsupported_operators!(
        sub => "-",
        mul => "*",
        add => "+",
        div => "/",
        extra_div => "//",
        modd => "%",
        gt => ">",
        lt => "<",
        eq => "==",
        ne => "!=",
        ge => ">=",
        le => "<=",
        and => "&&",
        or => "||",
        power => "**",
        bit_and => "&",
        bit_or => "|",
        xor => "^",
        bit_left_shift => "<<",
        bit_right_shift => ">>"
    );

    fn not(&self) -> RuntimeResult<Box<dyn TrcObj>> {
        Err(ErrorInfo::new(
            t!(
                OPERATOR_IS_NOT_SUPPORT,
                "0" = "!",
                "1" = self.get_type_name()
            ),
            t!(OPERATOR_ERROR),
        ))
    }

    fn bit_not(&self) -> RuntimeResult<Box<dyn TrcObj>> {
        Err(ErrorInfo::new(
            t!(
                OPERATOR_IS_NOT_SUPPORT,
                "0" = "~",
                "1" = self.get_type_name()
            ),
            t!(OPERATOR_ERROR),
        ))
    }

    fn self_negative(&self) -> RuntimeResult<Box<dyn TrcObj>> {
        Err(ErrorInfo::new(
            t!(
                OPERATOR_IS_NOT_SUPPORT,
                "0" = "-",
                "1" = self.get_type_name()
            ),
            t!(OPERATOR_ERROR),
        ))
    }

    fn get_type_name(&self) -> &str;
}

impl_downcast!(TrcObj);

gen_interface!(add, 2);
gen_interface!(sub, 2);
gen_interface!(mul, 2);
gen_interface!(div, 2);
gen_interface!(extra_div, 2);
gen_interface!(modd, 2);
gen_interface!(power, 2);
gen_interface!(gt, 2);
gen_interface!(lt, 2);
gen_interface!(eq, 2);
gen_interface!(ne, 2);
gen_interface!(ge, 2);
gen_interface!(le, 2);
gen_interface!(and, 2);
gen_interface!(or, 2);
gen_interface!(bit_and, 2);
gen_interface!(bit_or, 2);
gen_interface!(xor, 2);
gen_interface!(bit_left_shift, 2);
gen_interface!(bit_right_shift, 2);
gen_interface!(self_negative, 1);
gen_interface!(not, 1);
gen_interface!(bit_not, 1);
