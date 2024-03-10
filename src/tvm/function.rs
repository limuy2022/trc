use crate::base::func;

/// A context structure which hold the running info of the function
pub struct Frame<'a> {
    name: &'a str,
}

impl<'a> Frame<'a> {
    fn new(func: &'a func::Func) -> Self {
        Self { name: &func.name }
    }
}
