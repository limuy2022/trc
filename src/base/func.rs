#[derive(Clone)]
pub struct Func {
    pub func_addr: usize,
}

impl Func {
    pub fn new(name: usize) -> Self {
        Self { func_addr: name }
    }
}
