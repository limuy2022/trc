use super::func;
use core::cmp::max;
use std::fmt::Display;

#[derive(Debug)]
pub enum Opcode {
    Add,
    Sub,
    Mul,
    Div,
    ExtraDiv,
    Mod,
    Power,
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,
    And,
    Or,
    Not,
    Xor,
    BitNot,
    BitAnd,
    BitOr,
    BitLeftShift,
    BitRightShift,
    // change pc counter
    Goto,
    // return from a function
    PopFrame,
    // create a frame to hold the function
    NewFrame,
    // Load a int from const pool
    LoadInt,
    // Load a float from const pool
    LoadFloat,
    // Load a string from const pool
    LoadString,
    // Load a bigint from const pool
    LoadBigInt,
    // Load a local var to the stack
    LoadLocal,
    // Store a local var
    StoreLocal,
}

impl Display for Opcode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

pub struct ConstPool {
    pub intpool: Vec<i64>,
    pub stringpool: Vec<String>,
    pub floatpool: Vec<f64>,
}

impl ConstPool {
    pub fn new() -> Self {
        Self {
            intpool: Vec::new(),
            stringpool: Vec::new(),
            floatpool: Vec::new(),
        }
    }
}

pub struct Inst {
    pub opcode: Opcode,
    pub operand: usize,
}

impl Inst {
    pub fn new(opcode: Opcode, operand: usize) -> Self {
        Self { opcode, operand }
    }
}

pub struct StaticData {
    pub constpool: ConstPool,
    pub inst: Vec<Inst>,
    pub funcs: Vec<func::Func>,
    pub sym_table_sz: usize,
}

impl StaticData {
    pub fn new() -> StaticData {
        Self {
            constpool: ConstPool::new(),
            inst: vec![],
            funcs: vec![],
            sym_table_sz: 0,
        }
    }

    pub fn update_sym_table_sz(&mut self, newsz: usize) {
        self.sym_table_sz = max(self.sym_table_sz, newsz);
    }
}
