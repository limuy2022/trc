use crate::base::error;
use downcast_rs::{impl_downcast, Downcast};
use gettextrs::gettext;

pub mod data_structure;
pub mod trcbool;
pub mod trcfloat;
pub mod trcint;
pub mod trcstr;

macro_rules! unsupported_operator {
    ($operator_name:expr, $sself:expr) => {
        Err(error::ErrorInfo::new(
            gettext!(
                error::OPERATOR_IS_NOT_SUPPORT,
                $operator_name,
                $sself.get_type_name()
            ),
            error::SYNTAX_ERROR,
        ))
    };
}

/// help to generate the same error reporter functions
macro_rules! batch_unsupported_operators {
    ($($traie_name:ident => $oper_name:expr),*) => {
        $(fn $traie_name(&self, _ :Box<dyn TrcObj>) -> TypeError {
            unsupported_operator!($oper_name, self)
        })*
    };
}

#[macro_export]
macro_rules! impl_oper {
    // for unsupported operator in rust
    ($trait_oper_fn_name:ident, $oper:ident, $error_oper_name:expr, $self_type:ident, $newtype:ident, $whether_throw_error:tt) => {
        fn $trait_oper_fn_name(&self, other:Box<dyn TrcObj>) -> TypeError {
            match other.downcast_ref::<$self_type>() {
                Some(v) => {
                    Ok(Box::new($newtype::new($oper(self.value, v.value)$whether_throw_error)))
                },
                None => {
                    Err(ErrorInfo::new(gettext!(OPERATOR_IS_NOT_SUPPORT, $error_oper_name, other.get_type_name()), OPERATOR_ERROR))
                }
            }
        }
    };
    // for supported operator in rust
    ($trait_oper_fn_name:ident, $oper:tt, $error_oper_name:expr, $self_type:ident, $newtype:ident) => {
        fn $trait_oper_fn_name(&self, other:Box<dyn TrcObj>) -> TypeError {
            match other.downcast_ref::<$self_type>() {
                Some(v) => {
                    Ok(Box::new($newtype::new(self.value $oper v.value)))
                },
                None => {
                    Err(ErrorInfo::new(gettext!(OPERATOR_IS_NOT_SUPPORT, $error_oper_name, other.get_type_name()), OPERATOR_ERROR))
                }
            }
        }
    };
}

#[macro_export]
/// use tvm::types::batch_impl_opers;
/// batch_impl_opers!(
/// add => +, "+", TrcInt, TrcInt,
/// sub => -, "-", TrcInt, TrcInt,
/// mul => *, "*", TrcInt, TrcInt
/// );
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
        fn $trait_oper_fn_name(&self) -> TypeError {
            Ok(Box::new($newtype::new($oper self.value)))
        }
    };
}

type TypeError = Result<Box<dyn TrcObj>, error::ErrorInfo>;

pub trait TrcObj: Downcast + std::fmt::Display {
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
        and => "and",
        or => "or",
        power => "**",
        bit_and => "&",
        bit_or => "|",
        xor => "~",
        bit_left_shift => "<<",
        bit_right_shift => ">>"
    );

    fn not(&self) -> TypeError {
        unsupported_operator!("not", self)
    }

    fn bit_not(&self) -> TypeError {
        unsupported_operator!("xor", self)
    }

    fn get_type_name(&self) -> &str;
}

impl_downcast!(TrcObj);
