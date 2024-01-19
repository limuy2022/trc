mod algo;
mod function;
mod types;
mod def;

use clap::error;
use gettextrs::gettext;

use crate::{
    base::error::{ErrorContent, report_error, ErrorInfo, VM_DATA_NUMBER, VM_ERROR},
    cfg,
};

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

pub struct DynaData<'a> {
    obj_stack: Vec<Box<dyn types::TrcObj>>,
    frames_stack: Vec<function::Frame<'a>>,
}

impl<'a> DynaData<'a> {
    pub fn new() -> Self {
        Self {
            obj_stack: Vec::new(),
            frames_stack: vec![],
        }
    }
}

pub struct Inst {
    opcode: Opcode,
    operand: i32,
}

pub struct Vm<'a> {
    constpool: ConstPool,
    inst: Vec<Inst>,
    funcs: Vec<function::Func>,
    run_contnet: Content,
    dynadata: DynaData<'a>,
    pc: usize,
}

struct Content {
    module_name: String,
    line_pos: usize,
}

impl ErrorContent for Content {
    fn get_module_name(&self) -> &str {
        &self.module_name
    }

    fn get_line(&self) -> usize {
        self.line_pos
    }
}

impl Content {
    fn new(module_name: &str) -> Self {
        Content {
            module_name: String::from(module_name),
            line_pos: 0,
        }
    }

    fn add_line(&mut self) {
        self.line_pos += 1;
    }

    fn del_line(&mut self) {
        self.line_pos -= 1;
    }
}

enum Opcode {
    Add,
    Sub,
    Mul,
    Div,
    ExtraDiv,
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
    // change the option code index
    Goto,
    // return from a function
    PopFrame,
    // create a frame to hold the function
    NewFrame,
    // Load a int from const pool
    LoadInt
}

/// reduce the duplicate code to solve the operator running
macro_rules! OP {
    ($trait_used:ident, $sself:expr) => {{
        let t1 = $sself.dynadata.obj_stack.pop();
        let t2 = $sself.dynadata.obj_stack.pop();
        if t1.is_none() || t2.is_none() {
            report_error(&$sself.run_contnet, ErrorInfo::new(gettext!(VM_DATA_NUMBER, 2), VM_ERROR));
        }
        let t1 = t1.unwrap();
        let t2 = t2.unwrap();
        let ret = t1.$trait_used(t2);
        match ret {
            Err(e) => {
                report_error(&$sself.run_contnet, e);
            },
            Ok(t) => {
                $sself.dynadata.obj_stack.push(t);
            }
        }
    }};
}

impl<'a> Vm<'a> {
    pub fn new() -> Self {
        Self {
            constpool: ConstPool::new(),
            inst: Vec::new(),
            pc: 0,
            funcs: vec![],
            dynadata: DynaData::new(),
            run_contnet: Content::new(cfg::MAIN_MODULE_NAME),
        }
    }

    pub fn run(&mut self) {
        while self.pc < self.inst.len() {
            match self.inst[self.pc].opcode {
                Opcode::Add => OP!(add, self),
                Opcode::Sub => OP!(sub, self),
                Opcode::Mul => OP!(mul, self),
                Opcode::Div => OP!(div, self),
                Opcode::ExtraDiv => OP!(extra_div, self),
                Opcode::Mod => OP!(modd, self),
                Opcode::Gt => OP!(gt, self),
                Opcode::Lt => OP!(lt, self),
                Opcode::Ge => OP!(ge, self),
                Opcode::Le => OP!(le, self),
                Opcode::Eq => OP!(eq, self),
                Opcode::Ne => OP!(ne, self),
                Opcode::And => OP!(and, self),
                Opcode::Or => OP!(or, self),
                Opcode::NewFrame => {}
                Opcode::PopFrame => {
                    self.dynadata.frames_stack.pop();
                }
                _ => {
                    panic!("unknown opcode");
                }
            }
            self.pc += 1;
        }
    }
}
