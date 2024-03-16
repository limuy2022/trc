/// A context structure which hold the running info of the function
pub struct Frame {
    pub prev_addr: usize,
}

impl Frame {
    pub fn new(prev_addr: usize) -> Self {
        Self { prev_addr }
    }
}
