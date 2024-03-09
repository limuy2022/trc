#[derive(Clone)]
pub struct Func {
    pub name: String,
}

impl Func {
    fn new(name: String) -> Self {
        Self { name }
    }
}
