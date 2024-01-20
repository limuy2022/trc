mod algo;
mod def;
mod function;
mod types;

use clap::error;
use gettextrs::gettext;

use crate::{
    base::error::{report_error, ErrorContent, ErrorInfo, VM_DATA_NUMBER, VM_ERROR},
    cfg,
};

use self::types::trcint::TrcInt;

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
    operand: usize,
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
    // change the option code index
    Goto,
    // return from a function
    PopFrame,
    // create a frame to hold the function
    NewFrame,
    // Load a int from const pool
    LoadInt,
}

/// reduce the duplicate code to solve the operator running
macro_rules! binary_opcode {
    ($trait_used:ident, $sself:expr) => {{
        let t1 = $sself.dynadata.obj_stack.pop();
        let t2 = $sself.dynadata.obj_stack.pop();
        if t1.is_none() || t2.is_none() {
            report_error(
                &$sself.run_contnet,
                ErrorInfo::new(gettext!(VM_DATA_NUMBER, 2), VM_ERROR),
            );
        }
        let t1 = t1.unwrap();
        let t2 = t2.unwrap();
        let ret = t1.$trait_used(t2);
        match ret {
            Err(e) => {
                report_error(&$sself.run_contnet, e);
            }
            Ok(t) => {
                $sself.dynadata.obj_stack.push(t);
            }
        }
    }};
}

macro_rules! unary_opcode {
    ($trait_used:ident, $sself:expr) => {{
        let t1 = $sself.dynadata.obj_stack.pop();
        if t1.is_none() {
            report_error(
                &$sself.run_contnet,
                ErrorInfo::new(gettext!(VM_DATA_NUMBER, 1), VM_ERROR),
            );
        }
        let t1 = t1.unwrap();
        let ret = t1.$trait_used();
        match ret {
            Err(e) => {
                report_error(&$sself.run_contnet, e);
            }
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
                Opcode::Add => binary_opcode!(add, self),
                Opcode::Sub => binary_opcode!(sub, self),
                Opcode::Mul => binary_opcode!(mul, self),
                Opcode::Div => binary_opcode!(div, self),
                Opcode::ExtraDiv => binary_opcode!(extra_div, self),
                Opcode::Mod => binary_opcode!(modd, self),
                Opcode::Gt => binary_opcode!(gt, self),
                Opcode::Lt => binary_opcode!(lt, self),
                Opcode::Ge => binary_opcode!(ge, self),
                Opcode::Le => binary_opcode!(le, self),
                Opcode::Eq => binary_opcode!(eq, self),
                Opcode::Ne => binary_opcode!(ne, self),
                Opcode::And => binary_opcode!(and, self),
                Opcode::Or => binary_opcode!(or, self),
                Opcode::Power => binary_opcode!(power, self),
                Opcode::Not => unary_opcode!(not, self),
                Opcode::Xor => binary_opcode!(xor, self),
                Opcode::NewFrame => {}
                Opcode::PopFrame => {
                    self.dynadata.frames_stack.pop();
                }
                Opcode::Goto => {
                    self.pc = self.inst[self.pc].operand;
                }
                Opcode::LoadInt => {
                    // self.dynadata.obj_stack.push(self.constpool(self.inst[self.pc].u));
                    self.dynadata.obj_stack.push(Box::new(TrcInt::new(
                        self.constpool.intpool[self.inst[self.pc].operand],
                    )));
                }
                Opcode::BitAnd => binary_opcode!(bit_and, self),
                Opcode::BitOr => binary_opcode!(bit_or, self),
                Opcode::BitNot => unary_opcode!(bit_not, self),
            }
            self.pc += 1;
        }
    }
}
