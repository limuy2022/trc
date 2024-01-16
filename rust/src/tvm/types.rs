use crate::base::error;
use gettextrs::gettext;

mod data_structure;
mod trcfloat;
mod trcint;
mod trcstr;

pub trait TrcObj {
    fn output(&self) {}

    fn add(&self, _: Box<dyn TrcObj>) -> Result<Box<dyn TrcObj>, error::ErrorInfo> {
        Err(error::ErrorInfo::new(
            gettext!(error::OPERATOR_IS_NOT_SUPPORT, "+", self.get_type_name()),
            error::SYNTAX_ERROR,
        ))
    }

    fn get_type_name(&self) -> &str;
}
