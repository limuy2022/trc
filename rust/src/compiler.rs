//! reference iterator:https://stackoverflow.com/questions/43952104/how-can-i-store-a-chars-iterator-in-the-same-struct-as-the-string-it-is-iteratin
//! reference float hash map:https://www.soinside.com/question/tUJxYmevbVSHZYe2C2AK5o

mod ast;
mod token;

use self::token::TokenLex;
use crate::base::error;
use crate::cfg;
use crate::tvm::ConstPool;
use std::collections::hash_map;
use std::io::BufRead;
use std::{fs, io, vec};

#[derive(Debug)]
pub enum InputSource {
    File(String),
    StringInternal,
}

pub struct Option {
    optimize: bool,
    inputsource: InputSource,
}

#[derive(Debug, Clone)]
pub struct Content {
    module_name: String,
    line: usize,
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
    pub fn new(module_name: &str) -> Self {
        Self {
            module_name: String::from(module_name),
            line: 1,
        }
    }

    pub fn new_line(module_name: &str, line: usize) -> Self {
        Self {
            module_name: String::from(module_name),
            line,
        }
    }

    pub fn add_line(&mut self) {
        self.line += 1;
    }

    pub fn del_line(&mut self) {
        self.line -= 1;
    }

    pub fn set_line(&mut self, line: usize) {
        self.line = line;
    }
}

impl Option {
    pub fn new(optimize: bool, source: InputSource) -> Self {
        Option {
            optimize,
            inputsource: source,
        }
    }
}

#[derive(Hash, Eq, PartialEq)]
pub struct Float {
    front: i32,
    back: i32,
}

impl Float {
    fn new(front: i32, back: i32) -> Self {
        Self { front, back }
    }
}

pub struct ValuePool {
    const_ints: hash_map::HashMap<i64, usize>,
    const_strings: hash_map::HashMap<String, usize>,
    const_floats: hash_map::HashMap<Float, usize>,
}

const INT_VAL_POOL_ZERO: usize = 0;
const INT_VAL_POOL_ONE: usize = 1;

impl ValuePool {
    fn new() -> Self {
        let mut ret = Self {
            const_ints: hash_map::HashMap::new(),
            const_floats: hash_map::HashMap::new(),
            const_strings: hash_map::HashMap::new(),
        };
        ret.add_int(0);
        ret.add_int(1);
        ret
    }

    fn add_int(&mut self, val: i64) -> usize {
        let len_tmp = self.const_ints.len();
        *self.const_ints.entry(val).or_insert(len_tmp)
    }

    fn add_string(&mut self, val: String) -> usize {
        let len_tmp = self.const_strings.len();
        *self.const_strings.entry(val).or_insert(len_tmp)
    }

    fn add_float(&mut self, val: Float) -> usize {
        let len_tmp = self.const_floats.len();
        *self.const_floats.entry(val).or_insert(len_tmp)
    }

    fn store_val_to_vm(&mut self) -> ConstPool {
        let mut ret = ConstPool::new();
        ret.intpool.resize(self.const_ints.len(), 0);
        for i in &self.const_ints {
            ret.intpool[*i.1] = *i.0;
        }

        ret
    }
}

#[derive(Debug)]
pub struct StringSource {
    text: String,
    pos: usize,
    prev_size: usize,
}

impl StringSource {
    fn new(source: String) -> Self {
        Self {
            text: source,
            pos: 0,
            prev_size: 0,
        }
    }
}

impl TokenIo for StringSource {
    fn unread(&mut self, c: char) {
        self.pos -= self.prev_size;
        // check if match the right char
        if cfg!(debug_assertions) {
            assert_eq!(self.text[self.pos..].chars().next().unwrap(), c);
        }
    }

    fn read(&mut self) -> char {
        let mut opt = self.text[self.pos..].chars();
        match opt.next() {
            None => '\0',
            Some(c) => {
                let sz = c.len_utf8();
                self.prev_size = sz;
                self.pos += sz;
                c
            }
        }
    }
}

trait TokenIo {
    fn unread(&mut self, c: char);

    fn read(&mut self) -> char;
}

pub struct FileSource {
    back: Vec<char>,
    buf: io::BufReader<fs::File>,
    s: String,
    input_pos: usize,
}

impl FileSource {
    pub fn new(f: fs::File) -> Self {
        let buf = io::BufReader::new(f);
        let s = String::new();
        FileSource {
            back: vec![],
            buf,
            input_pos: 0,
            s,
        }
    }
}

impl TokenIo for FileSource {
    fn unread(&mut self, c: char) {
        self.back.push(c);
    }

    fn read(&mut self) -> char {
        if !self.back.is_empty() {
            return self.back.pop().unwrap();
        }
        loop {
            let mut input_pos = self.s[self.input_pos..].chars();
            match input_pos.next() {
                None => {
                    self.s.clear();
                    self.buf.read_line(&mut self.s).unwrap();
                    self.input_pos = 0;
                }
                Some(c) => {
                    self.input_pos += c.len_utf8();
                    return c;
                }
            }
        }
    }
}

pub struct Compiler {
    // to support read from stdin and file
    input: Box<dyn TokenIo>,
    const_pool: ValuePool,
    option: Option,
    content: Content,
}

impl Compiler {
    pub fn new(option: Option) -> Self {
        match option.inputsource {
            InputSource::File(ref filename) => {
                let f = std::fs::File::open(filename);
                Compiler {
                    input: Box::new(FileSource::new(f.unwrap())),
                    const_pool: ValuePool::new(),
                    option,
                    content: Content::new(cfg::MAIN_MODULE_NAME),
                }
            }
            _ => {
                panic!("Compiler construct Error, used {:?}", option.inputsource);
            }
        }
    }

    pub fn new_string_compiler(option: Option, source: &str) -> Self {
        Compiler {
            input: Box::new(StringSource::new(String::from(source))),
            const_pool: ValuePool::new(),
            option,
            content: Content::new(cfg::MAIN_MODULE_NAME),
        }
    }

    pub fn lex(&mut self) {
        let token_lexer = TokenLex::new(self);
        let ast_builder = ast::AstBuilder::new(token_lexer);
    }
}
