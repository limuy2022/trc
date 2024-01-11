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

pub struct DynaData {
    obj_stack: Vec<Box<dyn types::TrcObj>>
}

impl DynaData {
    pub fn new() -> Self {
        Self {
            obj_stack: Vec::new(),
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
    dynadata: DynaData,
    pc:usize
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
            pc:0,
            dynadata: DynaData::new()
        }
    }

    pub fn run(&mut self) {
        while self.pc < self.inst.len() {
            match self.inst[self.pc].opcode {
                Opcode::Add => {
                    
                },
                Opcode::Div => {

                },
                Opcode::Gt => {
                    
                }
                _ => {
                    panic!("unknown opcode");
                }
            }
            self.pc += 1;
        }
    }
}
