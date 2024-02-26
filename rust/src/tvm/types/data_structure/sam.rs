use crate::base::error::*;
use crate::base::stdlib::{OverrideWrapper, RustFunction};
use crate::tvm::types::trcchar::TrcChar;
use crate::tvm::types::TrcObj;
use crate::tvm::DynaData;
use crate::{compiler::token::TokenType, hash_map};
use derive::{trc_class, trc_function, trc_method};
use std::collections::HashMap;
use std::fmt::Display;

#[derive(Debug, Clone)]
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

#[trc_class]
#[derive(Debug, Clone)]
pub struct Sam {
    pub _states: Vec<Node>,
}

impl Sam {
    fn override_export() -> HashMap<TokenType, OverrideWrapper> {
        hash_map![]
    }

    pub fn new() -> Sam {
        Sam {
            _states: vec![Node::new(0)],
        }
    }
}

#[trc_method]
impl Sam {
    #[trc_function(method = true)]
    pub fn extend(s: Sam, c: TrcChar) {
        let id: usize = s._states.len();
        // 后缀自动机最后一个节点
        let mut last = id - 1;
        // 新节点，创造是因为包括最后一个字符串之后一定生成了一个新的等价类，也就是整个字符串，而它的长度一定等于上一个节点的长度加一
        let mut u = Node::new(s._states[last].len + 1);

        loop {
            if !s._states[last].next.contains_key(&c._value) {
                s._states[last].next.insert(c._value, id);
            } else {
                break;
            }
            match s._states[last].link {
                None => {
                    break;
                }
                Some(ind) => {
                    last = ind;
                }
            }
        }
        if let None = s._states[last].link {
            u.link = Some(0);
            s._states.push(u);
        } else {
            let q = s._states[last].next[&c._value];
            if s._states[q].len == s._states[last].len + 1 {
                u.link = Some(q);
                s._states.push(u);
            } else {
                let mut clone = Node::new(s._states[last].len + 1);
                clone.next = s._states[q].next.clone();
                clone.link = s._states[q].link;
                let cloneid = id + 1;
                loop {
                    if let Some(tmp) = s._states[last].next.get_mut(&c._value) {
                        if *tmp != q {
                            break;
                        }
                        *tmp = cloneid;
                    } else {
                        break;
                    }
                    match s._states[last].link {
                        None => {
                            break;
                        }
                        Some(ind) => {
                            last = ind;
                        }
                    }
                }
                s._states[q].link = Some(cloneid);
                u.link = Some(cloneid);
                s._states.push(u);
                s._states.push(clone);
            }
        }
    }
}

impl TrcObj for Sam {
    fn get_type_name(&self) -> &str {
        "sam"
    }
}

impl Display for Sam {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self._states)
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
            // sam.extend(c);
        }
        assert_eq!(sam._states.len(), 334);
    }
}
