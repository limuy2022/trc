mod token;

use std::io;
use std::collections::hash_set;

pub struct ConstPool {
    const_ints: hash_set::HashSet<i32>,
}

impl ConstPool {
    fn new() -> Self {
        ConstPool {
            const_ints: hash_set::HashSet::new(),
        }
    }
}

pub struct Compiler<T:io::Read> {
    input:T,
    line: usize,
    const_pool: ConstPool
}

impl<T:io::Read> Compiler<T> {
    fn new(f: T) -> Self {
        Compiler {
            input:f,
            line:1,
            const_pool: ConstPool::new(),
        }
    }
}
