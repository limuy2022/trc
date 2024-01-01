use std::collections::HashMap;

pub struct Node {
    link: Option<usize>,
    next: HashMap<char, usize>,
    len: usize,
}

impl Node {
    pub fn new(len: usize) -> Node {
        Node {
            len,
            link: None,
            next: HashMap::new(),
        }
    }
}

pub struct Sam {
    states: Vec<Node>,
}

impl Sam {
    pub fn new() -> Sam {
        Sam {
            states: vec![Node::new(0)],
        }
    }

    pub fn extend(&mut self, c: char) {
        let id: usize = self.states.len();
        let mut last = id - 1;
        let mut u = Node::new(self.states[last].len + 1);
        loop {
            if !self.states[last].next.contains_key(&c) {
                self.states[last].next.insert(c, id);
            } else {
                break;
            }
            match self.states[last].link {
                None => {
                    break;
                }
                Some(ind) => {
                    last = ind;
                }
            }
        }
        if let None = self.states[last].link {
            u.link = Some(0);
            self.states.push(u);
        } else {
            let q = self.states[last].next[&c];
            if self.states[q].len == self.states[last].len + 1 {
                u.link = Some(q);
                self.states.push(u);
            } else {
                let mut clone = Node::new(self.states[last].len + 1);
                clone.next = self.states[q].next.clone();
                clone.link = self.states[q].link;
                let cloneid = id + 1;
                loop {
                    if let Some(tmp) = self.states[last].next.get_mut(&c) {
                        if *tmp != q {
                            break;
                        }
                        *tmp = cloneid;
                    } else {
                        break;
                    }
                    match self.states[last].link {
                        None => {
                            break;
                        }
                        Some(ind) => {
                            last = ind;
                        }
                    }
                }
                self.states[q].link = Some(cloneid);
                u.link = Some(cloneid);
                self.states.push(u);
                self.states.push(clone);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn test_sam() {
        let mut sam = Sam::new();
        let str1 = "jdoasjdbasdhasjhdsjashdjsahshdhajdhsajkhdajshdsjadsjgfajshdasjfgjashdasjkdhudhwuidhsahsjkdhjadzxbjxbcnnxmbfdhbgsdahjkshdjbasjvahfghghgasjkdhasjdbdkhvfadhvfahdasjkhdasjkgdasjkbjkbchkdfgjhkasdghasjkdhasjdbfasjkgaskjdhadkjfgashk";
        for c in str1.chars() {
            sam.extend(c);
        }
        assert_eq!(sam.states.len(), 334);
    }
}
