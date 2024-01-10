mod types;
mod algo;

pub struct ConstPool {
    intpool: Vec<i32>,
    stringpool: Vec<String>,
    floatpool: Vec<f64>,
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
    opcode: Opcode,
    operand: i32,
}

pub struct Vm {
    constpool: ConstPool,
    inst: Vec<Inst>,
}

enum Opcode {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,
    And,
    Or,
    Not,
    Goto,
}

impl Vm {
    pub fn new() -> Self {
        Self {
            constpool: ConstPool::new(),
            inst: Vec::new(),
        }
    }

    pub fn run(&mut self) {}
}
