use gettextrs::gettext;
use std::error::Error;
use std::fmt::{Debug, Display};

pub const SYNTAX_ERROR: &str = "SyntaxError";
pub const OPERATOR_ERROR: &str = "OperatorError";
pub const VM_ERROR: &str = "VmError";
pub const ZERO_DIVSION_ERROR: &str = "ZeroDivisionError";

pub const STRING_WITHOUT_END: &str = "this string should be ended with {}";
pub const UNMATCHED_BRACE: &str = "{} is unmatched";
pub const OPERATOR_IS_NOT_SUPPORT: &str = "operator {} is not supported for type {}";
pub const VM_DATA_NUMBER: &str =
    "The number of data of vm stack is not correct, should have {} data";
pub const VM_FRAME_EMPTY: &str = "frame stack is empty.But running a pop frame opcode";
pub const ZERO_DIV: &str = "{} is divided by zero";

#[derive(Debug)]
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

pub trait ErrorContent: Debug + Send + Sync {
    fn get_module_name(&self) -> &str;

    fn get_line(&self) -> usize;
}

#[derive(Debug)]
pub struct RuntimeError {
    content: Box<dyn ErrorContent>,
    info: ErrorInfo,
}

impl Error for RuntimeError {}

impl Display for RuntimeError {
    /// report error in vm or compiler
    /// we will translate the error type to gettextrs
    /// but you should translate the error messgae by caller
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let s = format!(
            r#"Error in line {}
In module {}
{}:{}"#,
            self.content.get_line(),
            self.content.get_module_name(),
            gettext(self.info.errot_type),
            self.info.message
        );
        write!(f, "{}", s)
    }
}

impl RuntimeError {
    pub fn new(content: Box<dyn ErrorContent>, info: ErrorInfo) -> RuntimeError {
        RuntimeError { content, info }
    }
}
