//! This is Ac Automaton in Trc, a Structure for string matching

use std::collections::HashMap;

pub struct State {
    fail: usize,
    next: HashMap<char, usize>,
    show_times: usize,
}

impl State {
    fn new() -> Self {
        Self {
            fail: 0,
            next: HashMap::new(),
            show_times: 0,
        }
    }
}

pub struct AcAutomaton {
    states: Vec<State>,
}

impl AcAutomaton {
    fn new() -> AcAutomaton {
        AcAutomaton {
            states: vec![State::new()],
        }
    }

    fn add_string(&mut self, addstring: &str) {
        let mut u = 0;
        for i in addstring.chars() {
            if !self.states[u].next.contains_key(&i) {
                self.states.push(State::new());
                let new_node_id = self.states.len() - 1;
                self.states[u].next.insert(i, new_node_id);
            }
            u = self.states[u].next[&i];
        }
        self.states[u].show_times += 1;
    }

    // TODO this function use clone to build fail,it should be improved
    fn build_fail(&mut self) {
        let mut q: Vec<usize> = Vec::new();
        for i in self.states.first().unwrap().next.values() {
            q.push(*i);
        }
        while !q.is_empty() {
            let u = q.pop().unwrap();
            for (c, val) in &self.states[u].next.clone() {
                // 在这里需要向上找到失配指针
                // 正常的ac自动机会将剩余的失配部分也指向失配指针
                // 但是这个字符集被设计为无限大，可以容纳unicode的ac自动机，所以不能这么做
                // 会在匹配时顺着向上找失配指针
                self.states[*val].fail = if self.states[self.states[u].fail].next.contains_key(c) {
                    self.states[self.states[u].fail].next[c]
                } else {
                    0
                };
                q.push(*val);
            }
        }
    }

    /// use search function to add your string
    fn search(&self, _pattern: &str, _id: u32) {}

    fn get_ans(&self) -> HashMap<u32, usize> {
        HashMap::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn ac_automaton1() {
        let ac = AcAutomaton::new();
    }
}
