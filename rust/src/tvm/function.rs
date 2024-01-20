use super::types::TrcObj;

pub struct Func {
    name: String,
}

impl Func {
    fn new(name: String) -> Self {
        Self { name }
    }
}

/// A content structure which hold the running info of the function
pub struct Frame<'a> {
    name: &'a str,
}

impl<'a> Frame<'a> {
    fn new(func: &'a Func) -> Self {
        Self { name: &func.name }
    }
}
