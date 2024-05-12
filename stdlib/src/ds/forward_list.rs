pub struct ForwardList {
    sz: usize,
}

impl ForwardList {
    pub fn new() -> Self {
        Self { sz: 0 }
    }

    pub fn size(&self) -> usize {
        self.sz
    }

    pub fn empty(&self) -> bool {
        self.size() == 0
    }
}

impl Default for ForwardList {
    fn default() -> Self {
        Self::new()
    }
}
