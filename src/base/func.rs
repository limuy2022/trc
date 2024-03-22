#[derive(Clone)]
pub struct Func {
    pub func_addr: usize,
    pub var_table_sz: usize,
}

impl Func {
    pub fn new(name: usize, var_table_sz: usize) -> Self {
        Self {
            func_addr: name,
            var_table_sz,
        }
    }
}
