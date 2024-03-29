use crate::base::stdlib::{OverrideWrapper, RustFunction};
use crate::tvm::types::trcchar::TrcChar;
use crate::tvm::types::TrcObj;
use crate::{compiler::token::TokenType, hash_map};
use derive::{trc_class, trc_function, trc_method};
use std::collections::HashMap;
use std::fmt::Display;
use std::usize;

#[derive(Debug, Clone)]
pub struct Node {
    link: i32,
    next: HashMap<char, i32>,
    len: usize,
}

impl Node {
    pub fn new(len: usize) -> Node {
        Node {
            len,
            link: -1,
            next: HashMap::new(),
        }
    }
}

#[trc_class]
#[derive(Debug, Clone)]
pub struct Sam {
    pub _states: Vec<Node>,
    last: i32,
}

impl Sam {
    fn override_export() -> HashMap<TokenType, OverrideWrapper> {
        hash_map![]
    }

    pub fn new() -> Sam {
        Sam {
            _states: vec![Node::new(0)],
            last: 0,
        }
    }

    pub fn extend_internel(s: &mut Sam, c: char) {
        // 后缀自动机最后一个节点
        let mut p = s.last;
        // 新节点，创造是因为包括最后一个字符串之后一定生成了一个新的等价类，也就是整个字符串，而它的长度一定等于上一个节点的长度加一
        let mut newobj = Node::new(s._states[p as usize].len + 1);
        let cur = s._states.len() as i32;
        // 这一步是一直向上跳后缀链接，直到找到已经存在字符为c的转移，那么往上的节点一定也已经存在为c的转移了，可以停下
        while p != -1 && !s._states[p as usize].next.contains_key(&c) {
            s._states[p as usize].next.insert(c, cur);
            p = s._states[p as usize].link;
        }
        if p == -1 {
            // 一直跑到根节点了，直接新建节点即可
            newobj.link = 0;
            s._states.push(newobj);
        } else {
            let q = s._states[p as usize].next[&c];
            if s._states[q as usize].len == s._states[p as usize].len + 1 {
                // 这种是刚好对上字符串范围，直接指向即可
                newobj.link = q;
                s._states.push(newobj);
            } else {
                // 没能对上，分裂节点
                let mut clone = Node::new(s._states[p as usize].len + 1);
                clone.next.clone_from(&s._states[q as usize].next);
                clone.link = s._states[q as usize].link;
                let cloneid = cur + 1;
                while let Some(tmp) = s._states[p as usize].next.get_mut(&c) {
                    if *tmp != q {
                        break;
                    }
                    *tmp = cloneid;
                    p = s._states[p as usize].link;
                    if p == -1 {
                        break;
                    }
                }
                s._states[q as usize].link = cloneid;
                newobj.link = cloneid;
                s._states.push(newobj);
                s._states.push(clone);
            }
        }
        s.last = cur;
    }
}

#[trc_method]
impl Sam {
    #[trc_function(method = true)]
    pub fn extend(s: Sam, c: TrcChar) {
        Self::extend_internel(s, c._value);
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
    fn test_sam_easy() {
        let mut sam = Sam::new();
        for c in "abab".chars() {
            Sam::extend_internel(&mut sam, c);
        }
        assert_eq!(sam._states.len(), 5);
    }

    #[test]
    fn test_sam_easy2() {
        let mut sam = Sam::new();
        for c in "aabaa".chars() {
            Sam::extend_internel(&mut sam, c);
        }
        assert_eq!(sam._states.len(), 6);
    }

    #[test]
    fn test_sam() {
        let mut sam = Sam::new();
        let str1 = "jdoasjdbasdhasjhdsjashdjsahshdhajdhsajkhdajshdsjadsjgfajshdasjfgjashdasjkdhudhwuidhsahsjkdhjadzxbjxbcnnxmbfdhbgsdahjkshdjbasjvahfghghgasjkdhasjdbdkhvfadhvfahdasjkhdasjkgdasjkbjkbchkdfgjhkasdghasjkdhasjdbfasjkgaskjdhadkjfgashk";
        for c in str1.chars() {
            Sam::extend_internel(&mut sam, c);
        }
        assert_eq!(sam._states.len(), 334);
    }
}
