mod token;

use std::collections::hash_set;
use std::io;

pub struct Option {
    optimize:bool,
    file: String
}

impl Option {
    pub fn new(optimize:bool, file: String) -> Self {
        Option {
            optimize,
            file
        }
    }
}

pub struct ValuePool {
    const_ints: hash_set::HashSet<i32>,
}

impl ValuePool {
    fn new() -> Self {
        Self {
            const_ints: hash_set::HashSet::new(),
        }
    }
}

pub struct Compiler<T: io::Read> {
    input: T,
    line: usize,
    const_pool: ValuePool,
}

impl<T: io::Read> Compiler<T> {
    fn new(f: T) -> Self {
        Compiler {
            input: f,
            line: 1,
            const_pool: ValuePool::new(),
        }
    }
}
