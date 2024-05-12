struct Node {
    sons: [usize; 2],
    cnt: usize,
    sz: usize,
    fa: usize,
}

impl Node {
    fn _new() -> Self {
        Self {
            sons: [0; 2],
            sz: 0,
            cnt: 0,
            fa: 0,
        }
    }
}

pub struct Splay {
    tree: Vec<Node>,
}

impl Splay {
    pub fn new() -> Self {
        Self { tree: vec![] }
    }

    pub fn maintain(&mut self, id: usize) {
        self.tree[id].sz = self.tree[self.tree[id].sons[0]].sz
            + self.tree[self.tree[id].sons[1]].sz
            + self.tree[id].cnt;
    }

    pub fn is_right(&self, id: usize) -> bool {
        id == self.tree[self.tree[id].fa].sons[1]
    }
}

impl Default for Splay {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn splay() {}
}
