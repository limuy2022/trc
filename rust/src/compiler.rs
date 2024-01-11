mod token;

use std::collections::hash_set;
use std::io;
use crate::base::error;

/// 
enum InputSource {
    File(String),
    Stdin
}

pub struct Option {
    optimize:bool,
    filename: InputSource
}

pub struct Content {
    module_name:String,
    line:usize
}

impl error::ErrorContent for Content {
    fn get_module_name(&self) -> &str {
        &self.module_name
    }

    fn get_line(&self) -> usize {
        self.line
    }
}

impl Content {
    pub fn new(module_name:String) -> Self {
        Self {
            module_name,
            line:1
        }
    }

    pub fn add_line(&mut self) {
        self.line += 1;
    }

    pub fn del_line(&mut self) {
        self.line -= 1;
    }
}

impl Option {
    pub fn new(optimize:bool, filename: String) -> Self {
        Option {
            optimize,
            filename:InputSource::File(filename)
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

pub struct Compiler {
    // to support read from stdin and file
    input: Box<dyn io::Read>,
    line: usize,
    const_pool: ValuePool,
}

impl Compiler {
    fn new(option: Option) -> Self {
        match option.filename {
            InputSource::Stdin => {
                Compiler {
                    input: Box::new(io::stdin()),
                    line: 1,
                    const_pool: ValuePool::new(),
                }
            },
            InputSource::File(filename) => {
                let f = std::fs::File::open(filename);
                Compiler {
                    input: Box::new(f.unwrap()),
                    line: 1,
                    const_pool: ValuePool::new(),
                }
            }
        }
    }
}
