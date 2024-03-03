use colored::Colorize;
use rust_i18n::t;
use std::error::Error;
use std::fmt::{Debug, Display};

pub const SYNTAX_ERROR: &str = "compiler.SyntaxError";
pub const OPERATOR_ERROR: &str = "compiler.OperatorError";
pub const VM_ERROR: &str = "compiler.VmError";
pub const ZERO_DIVSION_ERROR: &str = "compiler.ZeroDivisionError";
pub const NUMBER_OVER_FLOW: &str = "compiler.NumberOverFlowError";
pub const SYMBOL_ERROR: &str = "compiler.SymbolError";
pub const TYPE_ERROR: &str = "compiler.TypeError";
pub const ARGUMENT_ERROR: &str = "compiler.ArgumentError";
pub const FORMAT_STR_ERROR: &str = "compiler.FormatStringError";

pub const STRING_WITHOUT_END: &str = "compiler.syntaxerror.str_error";
pub const CHAR_FORMAT: &str = "compiler.syntaxerror.char_error";
pub const EXPECTED_EXPR: &str = "compiler.syntaxerror.expected_expr";
pub const UNMATCHED_BRACE: &str = "compiler.syntaxerror.unmatched";
pub const OPERATOR_IS_NOT_SUPPORT: &str = "compiler.typerror.operator_unsupport";
pub const VM_DATA_NUMBER: &str = "vm.vmerror.data_num_error";
pub const VM_FRAME_EMPTY: &str = "vm.vmerror.stack_frame_empty";
pub const ZERO_DIV: &str = "vm.zerodivisionerror.zerodiv";
pub const PREFIX_FOR_FLOAT: &str = "compiler.syntaxerror.prefix_float";
pub const FLOAT_OVER_FLOW: &str = "compiler.numberoverflowerror.float_over_flow";
pub const UNEXPECTED_TOKEN: &str = "compiler.syntaxerror.unexpected_token";
pub const ERROR_IN_LINE: &str = "compiler.report.error_in_line";
pub const IN_MODULE: &str = "compiler.report.in_module";
pub const SYMBOL_NOT_FOUND: &str = "compiler.symbolerror.not_found_sym";
pub const SYMBOL_REDEFINED: &str = "compiler.symbolerror.redefined_sym";
pub const TYPE_NOT_THE_SAME: &str = "compiler.typerror.type_not_same";
pub const ARGU_NUMBER: &str = "compiler.argumenterror.argu_num";
pub const EXPECT_TYPE: &str = "compiler.argumenterror.type_unmatched";
pub const UNCLODED_COMMENT: &str = "compiler.syntaxerror.unclosed_comment";
pub const UNCLOSED_FORMAT: &str = "compiler.formatstringerror.unclosed_format";
pub const ARGUMENT_CANNOT_BE_VOID: &str = "compiler.argumenterror.void_argu";

#[derive(Debug)]
pub struct ErrorInfo {
    pub message: String,
    error_type: String,
}

impl ErrorInfo {
    pub fn new(message: impl Into<String>, error_type: impl Into<String>) -> ErrorInfo {
        ErrorInfo {
            message: message.into(),
            error_type: error_type.into(),
        }
    }
}

pub trait ErrorContext: Debug + Send + Sync {
    fn get_module_name(&self) -> &str;

    fn get_line(&self) -> usize;
}

#[derive(Debug)]
pub struct RuntimeError {
    context: Box<dyn ErrorContext>,
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
            t!(ERROR_IN_LINE, line = self.context.get_line()),
            t!(IN_MODULE, name = self.context.get_module_name()),
            self.info.error_type.clone().red(),
            self.info.message.red()
        );
        write!(f, "{}", s)
    }
}

impl RuntimeError {
    pub fn new(context: Box<dyn ErrorContext>, info: ErrorInfo) -> RuntimeError {
        // if info.message != "" {
        // panic!("develop debug use");
        // }
        RuntimeError { context, info }
    }
}

#[derive(Debug, Default)]
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

#[derive(Default)]
pub struct LightFakeContext {}

impl ErrorContext for LightFakeError {
    fn get_module_name(&self) -> &str {
        ""
    }
    fn get_line(&self) -> usize {
        0
    }
}

impl LightFakeContext {
    pub fn new() -> LightFakeContext {
        LightFakeContext {}
    }
}

impl From<LightFakeError> for RuntimeError {
    fn from(_: LightFakeError) -> RuntimeError {
        RuntimeError::new(Box::new(LightFakeError::new()), ErrorInfo::new("", ""))
    }
}

pub type RunResult<T> = Result<T, RuntimeError>;
pub type RuntimeResult<T> = Result<T, ErrorInfo>;
