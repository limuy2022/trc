use super::func;

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
    // change the option code index
    Goto,
    // return from a function
    PopFrame,
    // create a frame to hold the function
    NewFrame,
    // Load a int from const pool
    LoadInt,
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

pub struct StaticData {
    pub constpool: ConstPool,
    pub inst: Vec<Inst>,
    pub funcs: Vec<func::Func>,
}

impl StaticData {
    pub fn new() -> StaticData {
        Self {
            constpool: ConstPool::new(),
            inst: vec![],
            funcs: vec![],
        }
    }
}
