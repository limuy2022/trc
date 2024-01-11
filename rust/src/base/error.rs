enum ErrorType {
    SyntaxError,
    OperatorError
}

pub struct ErrorInfo {
    pub message: String,
    errot_type: ErrorType,
}

impl ErrorInfo {
    pub fn new(message: String) -> ErrorInfo {
        ErrorInfo {
            message,
            errot_type: ErrorType::SyntaxError,
        }
    }
}

pub trait ErrorContent {
    fn get_module_name(&self) -> &str;

    fn get_line(&self) -> usize;
}

pub fn report_error(content:impl ErrorContent, info:ErrorInfo) {

}
