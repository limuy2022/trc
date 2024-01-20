use crate::base::error;
use downcast_rs::{impl_downcast, Downcast};
use gettextrs::gettext;

pub mod data_structure;
pub mod trcfloat;
pub mod trcint;
pub mod trcstr;
pub mod trcbool;

macro_rules! unsupported_operator {
    ($operator_name:expr, $sself:expr) => {
        Err(error::ErrorInfo::new(
            gettext!(error::OPERATOR_IS_NOT_SUPPORT, $operator_name, $sself.get_type_name()),
            error::SYNTAX_ERROR,
        ))
    }
}

/// help to generate the same error reporter functions
macro_rules! operators {
    ($($traie_name:ident => $oper_name:expr),*) => {
        $(fn $traie_name(&self, _ :Box<dyn TrcObj>) -> TypeError {
            unsupported_operator!($oper_name, self)
        })*
    };
}

#[macro_export]
macro_rules! impl_oper {
    ($trait_oper_fn_name:ident, $oper:tt, $error_oper_name:expr, $self_type:ident) => {
        fn $trait_oper_fn_name(&self, other:Box<dyn TrcObj>) -> TypeError {
            match other.downcast_ref::<$self_type>() {
                Some(v) => {
                    Ok(Box::new(TrcInt::new(self.value $oper v.value)))
                },
                None => {
                    Err(ErrorInfo::new(gettext!(OPERATOR_IS_NOT_SUPPORT, $error_oper_name, other.get_type_name()), OPERATOR_ERROR))
                }
            }
        }
    };
}

#[macro_export]
macro_rules! batch_impl_opers {
    ($($trait_oper_fn_name:ident => $oper:tt, $error_oper_name:expr, $self_type:ident),*) => {
        $(
            impl_oper!($trait_oper_fn_name, $oper, $error_oper_name, $self_type);
        )*
    };
}

type TypeError= Result<Box<dyn TrcObj>, error::ErrorInfo>;

pub trait TrcObj:Downcast {
    fn output(&self) {}

    operators!(
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
        or => "or"
    );

    fn get_type_name(&self) -> &str;
}

impl_downcast!(TrcObj);
