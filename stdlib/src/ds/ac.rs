//! This is Ac Automaton in Trc, a Structure for string matching

use std::collections::HashMap;

pub struct State {
    _fail: usize,
    next: HashMap<char, usize>,
    show_times: usize,
}

impl State {
    fn new() -> Self {
        Self {
            _fail: 0,
            next: HashMap::new(),
            show_times: 0,
        }
    }
}

pub struct AcAutomaton {
    states: Vec<State>,
}

impl AcAutomaton {
    pub fn new() -> AcAutomaton {
        AcAutomaton {
            states: vec![State::new()],
        }
    }

    pub fn add_string(&mut self, addstring: &str) {
        let mut u = 0;
        for i in addstring.chars() {
            let condit = !self.states[u].next.contains_key(&i);
            if condit {
                self.states.push(State::new());
                let new_node_id = self.states.len() - 1;
                self.states[u].next.insert(i, new_node_id);
            }
            u = self.states[u].next[&i];
        }
        self.states[u].show_times += 1;
    }

    // TODO this function use clone to build fail,it should be improved
    pub fn build_fail(&mut self) {
        let mut q: Vec<usize> = Vec::new();
        for i in self.states.first().unwrap().next.values() {
            q.push(*i);
        }
        while let Some(u) = q.pop() {
            for (c, val) in &self.states[u].next.clone() {
                // 在这里需要向上找到失配指针
                // 正常的ac自动机会将剩余的失配部分也指向失配指针
                // 但是这个字符集被设计为无限大，可以容纳unicode的ac自动机，所以不能这么做
                // 会在匹配时顺着向上找失配指针
                self.states[*val]._fail = if self.states[self.states[u]._fail].next.contains_key(c)
                {
                    self.states[self.states[u]._fail].next[c]
                } else {
                    0
                };
                q.push(*val);
            }
        }
    }

    /// use search function to add your string
    pub fn search(&self, _pattern: &str, _id: u32) {}

    pub fn get_ans(&self) -> HashMap<u32, usize> {
        HashMap::new()
    }
}

impl Default for AcAutomaton {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn ac_automaton1() {
        let _ac = AcAutomaton::new();
    }
}
