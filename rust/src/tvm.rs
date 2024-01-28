mod algo;
mod def;
mod function;
mod gc;
mod types;

use self::types::trcfloat::TrcFloat;
use self::types::trcint::TrcInt;
use self::types::trcstr::TrcStr;
use crate::base::codegen::{self, StaticData};
use crate::{base::error::*, cfg};
use gettextrs::gettext;

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

pub struct Vm<'a> {
    run_contnet: Content,
    dynadata: DynaData<'a>,
    pc: usize,
    static_data: StaticData,
}

#[derive(Debug, Clone)]
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

/// reduce the duplicate code to solve the operator running
macro_rules! binary_opcode {
    ($trait_used:ident, $sself:expr) => {{
        let t1 = $sself.dynadata.obj_stack.pop();
        let t2 = $sself.dynadata.obj_stack.pop();
        if t1.is_none() || t2.is_none() {
            return Err(RuntimeError::new(
                Box::new($sself.run_contnet.clone()),
                ErrorInfo::new(gettext!(VM_DATA_NUMBER, 2), gettext(VM_ERROR)),
            ));
        }
        let t1 = t1.unwrap();
        let t2 = t2.unwrap();
        let ret = t1.$trait_used(t2);
        match ret {
            Err(e) => {
                return Err(RuntimeError::new(Box::new($sself.run_contnet.clone()), e));
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
            return Err(RuntimeError::new(
                Box::new($sself.run_contnet.clone()),
                ErrorInfo::new(gettext!(VM_DATA_NUMBER, 1), gettext(VM_ERROR)),
            ));
        }
        let t1 = t1.unwrap();
        let ret = t1.$trait_used();
        match ret {
            Err(e) => {
                return Err(RuntimeError::new(Box::new($sself.run_contnet.clone()), e));
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
            pc: 0,
            dynadata: DynaData::new(),
            run_contnet: Content::new(cfg::MAIN_MODULE_NAME),
            static_data: StaticData::new(),
        }
    }

    pub fn new_init(static_data: StaticData) -> Self {
        Self {
            pc: 0,
            dynadata: DynaData::new(),
            run_contnet: Content::new(cfg::MAIN_MODULE_NAME),
            static_data,
        }
    }

    pub fn run(&mut self) -> Result<(), RuntimeError> {
        while self.pc < self.static_data.inst.len() {
            match self.static_data.inst[self.pc].opcode {
                codegen::Opcode::Add => binary_opcode!(add, self),
                codegen::Opcode::Sub => binary_opcode!(sub, self),
                codegen::Opcode::Mul => binary_opcode!(mul, self),
                codegen::Opcode::Div => binary_opcode!(div, self),
                codegen::Opcode::ExtraDiv => binary_opcode!(extra_div, self),
                codegen::Opcode::Mod => binary_opcode!(modd, self),
                codegen::Opcode::Gt => binary_opcode!(gt, self),
                codegen::Opcode::Lt => binary_opcode!(lt, self),
                codegen::Opcode::Ge => binary_opcode!(ge, self),
                codegen::Opcode::Le => binary_opcode!(le, self),
                codegen::Opcode::Eq => binary_opcode!(eq, self),
                codegen::Opcode::Ne => binary_opcode!(ne, self),
                codegen::Opcode::And => binary_opcode!(and, self),
                codegen::Opcode::Or => binary_opcode!(or, self),
                codegen::Opcode::Power => binary_opcode!(power, self),
                codegen::Opcode::Not => unary_opcode!(not, self),
                codegen::Opcode::Xor => binary_opcode!(xor, self),
                codegen::Opcode::NewFrame => {}
                codegen::Opcode::PopFrame => {
                    let ret = self.dynadata.frames_stack.pop();
                    if let None = ret {
                        return Err(RuntimeError::new(
                            Box::new(self.run_contnet.clone()),
                            ErrorInfo::new(gettext(VM_FRAME_EMPTY), gettext(VM_ERROR)),
                        ));
                    }
                }
                codegen::Opcode::Goto => {
                    self.pc = self.static_data.inst[self.pc].operand;
                }
                codegen::Opcode::LoadInt => {
                    self.dynadata.obj_stack.push(Box::new(TrcInt::new(
                        self.static_data.constpool.intpool[self.static_data.inst[self.pc].operand],
                    )));
                }
                codegen::Opcode::BitAnd => binary_opcode!(bit_and, self),
                codegen::Opcode::BitOr => binary_opcode!(bit_or, self),
                codegen::Opcode::BitNot => unary_opcode!(bit_not, self),
                codegen::Opcode::BitLeftShift => binary_opcode!(bit_left_shift, self),
                codegen::Opcode::BitRightShift => binary_opcode!(bit_right_shift, self),
                codegen::Opcode::LoadLocal => {}
                codegen::Opcode::StoreLocal => {}
                codegen::Opcode::LoadString => {
                    self.dynadata.obj_stack.push(Box::new(TrcStr::new(
                        &self.static_data.constpool.stringpool
                            [self.static_data.inst[self.pc].operand],
                    )));
                }
                codegen::Opcode::LoadFloat => {
                    self.dynadata.obj_stack.push(Box::new(TrcFloat::new(
                        self.static_data.constpool.floatpool
                            [self.static_data.inst[self.pc].operand],
                    )));
                }
                codegen::Opcode::LoadBigInt => {}
            }
            self.pc += 1;
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn test_vm() {}
}
