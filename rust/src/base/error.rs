enum ErrorType {
    SyntaxError,
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
