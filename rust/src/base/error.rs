use gettextrs::gettext;
use std::process::exit;

const EXIT_FAILURE: i32 = 1;

pub const SYNTAX_ERROR: &str = "SyntaxError";
pub const OPERATOR_ERROR: &str = "OperatorError";
pub const VM_ERROR:&str = "VmError";

pub const STRING_WITHOUT_END: &str = "this string should be ended with {}";
pub const OPERATOR_IS_NOT_SUPPORT: &str = "operator {} is not supported for type {}";
pub const VM_DATA_NUMBER:&str = "The number of data of vm stack is not correct, should have {} data";

pub struct ErrorInfo {
    pub message: String,
    errot_type: &'static str,
}

impl ErrorInfo {
    pub fn new(message: String, error_type: &'static str) -> ErrorInfo {
        ErrorInfo {
            message,
            errot_type: error_type,
        }
    }
}

pub trait ErrorContent {
    fn get_module_name(&self) -> &str;

    fn get_line(&self) -> usize;
}

/// report error in vm or compiler
/// we will translate the error type to gettextrs
/// but you should translate the error messgae by caller
pub fn report_error(content: &impl ErrorContent, info: ErrorInfo) {
    eprintln!("Error in line {}", content.get_line());
    eprintln!("In module {}", content.get_module_name());
    eprintln!("{}:{}", gettext(info.errot_type), info.message);
    exit(EXIT_FAILURE);
}
