use gettextrs::gettext;
use std::error::Error;
use std::fmt::{Debug, Display};

pub const SYNTAX_ERROR: &str = "SyntaxError";
pub const OPERATOR_ERROR: &str = "OperatorError";
pub const VM_ERROR: &str = "VmError";
pub const ZERO_DIVSION_ERROR: &str = "ZeroDivisionError";
pub const NUMBER_OVER_FLOW: &str = "NumberOverFlowError";
pub const SYMBOL_ERROR: &str = "SymbolError";
pub const TYPE_ERROR: &str = "TypeError";
pub const ARGUMENT_ERROR: &str = "ArgumentError";

pub const STRING_WITHOUT_END: &str = "this string should be ended with {}";
pub const UNMATCHED_BRACE: &str = "{} is unmatched";
pub const OPERATOR_IS_NOT_SUPPORT: &str = "operator {} is not supported for type {}";
pub const VM_DATA_NUMBER: &str =
    "The number of data of vm stack is not correct, should have {} data";
pub const VM_FRAME_EMPTY: &str = "frame stack is empty.But running a pop frame opcode";
pub const ZERO_DIV: &str = "{} is divided by zero";
pub const PREFIX_FOR_FLOAT: &str = "Prefix {} can be used for float";
pub const FLOAT_OVER_FLOW: &str = "Float {} is too large to store";
pub const UNEXPECTED_TOKEN: &str = "token {} is not expected";
pub const ERROR_IN_LINE: &str = "Error in line {}";
pub const IN_MODULE: &str = "In module {}";
pub const SYMBOL_NOT_FOUND: &str = "Symbol {} not found";
pub const SYMBOL_REDEFINED: &str = "Symbol {} redefined";
pub const TYPE_NOT_THE_SAME: &str = "Type {} and {} are not the same";
pub const ARGU_NUMBER: &str = "expect {}.But given {}";
pub const EXPECT_TYPE: &str = "Expect type {}.But given type {}";

#[derive(Debug)]
pub struct ErrorInfo {
    pub message: String,
    error_type: String,
}

impl ErrorInfo {
    pub fn new(message: String, error_type: String) -> ErrorInfo {
        ErrorInfo {
            message,
            error_type,
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
            r#"{}
{}
{}:{}"#,
            gettext!(ERROR_IN_LINE, self.content.get_line()),
            gettext!(IN_MODULE, self.content.get_module_name()),
            gettext(self.info.error_type.clone()),
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

#[derive(Debug)]
pub struct LightFakeError {}

impl Error for LightFakeError {}

impl Display for LightFakeError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "")
    }
}

impl LightFakeError {
    pub fn new() -> LightFakeError {
        LightFakeError {}
    }
}

pub struct LightFakeContent {}

impl ErrorContent for LightFakeError {
    fn get_module_name(&self) -> &str {
        ""
    }
    fn get_line(&self) -> usize {
        0
    }
}

impl LightFakeContent {
    pub fn new() -> LightFakeContent {
        LightFakeContent {}
    }
}

impl From<LightFakeError> for RuntimeError {
    fn from(_: LightFakeError) -> RuntimeError {
        RuntimeError::new(
            Box::new(LightFakeError::new()),
            ErrorInfo::new("".to_string(), "".to_string()),
        )
    }
}

pub type RunResult<T> = Result<T, RuntimeError>;
