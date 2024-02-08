mod algo;
mod def;
mod function;
mod gc;
pub mod stdlib;
mod types;

use self::types::trcfloat::{div_float, exact_div_float, TrcFloat};
use self::types::trcint::{div_int, exact_div_int, mod_int, power_int, TrcInt};
use self::types::trcstr::TrcStr;
use self::types::{TrcBool, TrcChar, TrcObj};
use crate::base::codegen::{self, Opcode, StaticData};
use crate::base::stdlib::STD_FUNC_TABLE;
use crate::{base::error::*, cfg};
use gettextrs::gettext;
use libloading::Library;

#[derive(Default)]
pub struct DynaData<'a> {
    obj_stack: Vec<Box<dyn types::TrcObj>>,
    int_stack: Vec<i64>,
    str_stack: Vec<String>,
    float_stack: Vec<f64>,
    bool_stack: Vec<bool>,
    char_stack: Vec<char>,
    frames_stack: Vec<function::Frame<'a>>,
}

impl<'a> DynaData<'a> {
    pub fn new() -> Self {
        Self {
            ..Default::default()
        }
    }

    pub fn check_stack<T>(obj: &[T], require: usize) -> RuntimeResult<()> {
        if obj.len() < require {
            return Err(ErrorInfo::new(
                gettext!(VM_DATA_NUMBER, require),
                gettext(VM_ERROR),
            ));
        }
        Ok(())
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

    fn set_line(&mut self, newline: usize) {
        self.line_pos = newline;
    }
}

/// Load libc
fn load_libc() -> Option<&'static Library> {
    static LIBC: std::sync::OnceLock<Option<Library>> = std::sync::OnceLock::new();
    #[cfg(target_os = "linux")]
    {
        let tmp = LIBC.get_or_init(|| unsafe {
            let lib: Result<Library, libloading::Error> = Library::new("");
            match lib {
                Err(_) => None,
                Ok(val) => Some(val),
            }
        });
        match tmp {
            None => None,
            Some(val) => Some(val),
        }
    }
    #[cfg(target_os = "windows")]
    {
        let tmp = LIBC.get_or_init(|| unsafe {
            let lib: Result<Library, libloading::Error> = libloading::Library::new("msvcrt.dll");
            match lib {
                Err(_) => None,
                Ok(val) => Some(val),
            }
        });
        match tmp {
            None => None,
            Some(val) => Some(&val),
        }
    }
    #[cfg(target_os = "macos")]
    {
        let tmp = LIBC.get_or_init(|| unsafe {
            let lib: Result<Library, libloading::Error> = libloading::Library::new("libc.dylib");
            match lib {
                Err(_) => None,
                Ok(val) => Some(val),
            }
        });
        match tmp {
            None => None,
            Some(val) => Some(&val),
        }
    }
}

/// reduce the duplicate code to solve the operator running
macro_rules! operator_opcode {
    ($trait_used:ident, $sself:expr) => {{
        let ret = types::$trait_used(&mut $sself.dynadata);
        match ret {
            Err(e) => {
                return Err(RuntimeError::new(Box::new($sself.run_contnet.clone()), e));
            }
            Ok(_) => {}
        }
    }};
}
macro_rules! impl_opcode {
    ($obj_stack:expr, $sself:expr, 2) => {{
        let tmp = DynaData::check_stack(&$obj_stack, 2);
        $sself.throw_err_info(tmp)?;
        let second = $obj_stack.pop().unwrap();
        let first = $obj_stack.pop().unwrap();
        (first, second)
    }};
    ($obj_stack:expr, $sself:expr, 1) => {{
        let tmp = DynaData::check_stack(&$obj_stack, 1);
        $sself.throw_err_info(tmp)?;
        let first = $obj_stack.pop().unwrap();
        (first)
    }};
}

impl Default for Vm<'_> {
    fn default() -> Self {
        Self::new()
    }
}

impl<'a> Vm<'a> {
    pub fn new() -> Self {
        Self {
            pc: 0,
            dynadata: DynaData::new(),
            run_contnet: Content::new(cfg::MAIN_MODULE_NAME),
            static_data: StaticData::new(false),
        }
    }

    fn new_init(static_data: StaticData) -> Self {
        Self {
            pc: 0,
            dynadata: DynaData::new(),
            run_contnet: Content::new(cfg::MAIN_MODULE_NAME),
            static_data,
        }
    }

    pub fn set_static_data(&mut self, static_data: StaticData) {
        self.static_data = static_data;
    }

    fn throw_err_info<T>(&self, info: RuntimeResult<T>) -> RunResult<T> {
        match info {
            Ok(data) => Ok(data),
            Err(e) => Err(RuntimeError::new(Box::new(self.run_contnet.clone()), e)),
        }
    }

    pub fn run(&mut self) -> Result<(), RuntimeError> {
        while self.pc < self.static_data.inst.len() {
            if self.static_data.has_line_table {
                self.run_contnet
                    .set_line(self.static_data.line_table[self.pc]);
            }
            match self.static_data.inst[self.pc].opcode {
                Opcode::Add => operator_opcode!(add, self),
                Opcode::Sub => operator_opcode!(sub, self),
                Opcode::Mul => operator_opcode!(mul, self),
                Opcode::Div => operator_opcode!(div, self),
                Opcode::ExactDiv => operator_opcode!(extra_div, self),
                Opcode::Mod => operator_opcode!(modd, self),
                Opcode::Gt => operator_opcode!(gt, self),
                Opcode::Lt => operator_opcode!(lt, self),
                Opcode::Ge => operator_opcode!(ge, self),
                Opcode::Le => operator_opcode!(le, self),
                Opcode::Eq => operator_opcode!(eq, self),
                Opcode::Ne => {
                    operator_opcode!(ne, self)
                }
                Opcode::And => operator_opcode!(and, self),
                Opcode::Or => operator_opcode!(or, self),
                Opcode::Power => operator_opcode!(power, self),
                Opcode::Not => operator_opcode!(not, self),
                Opcode::Xor => operator_opcode!(xor, self),
                Opcode::NewFrame => {}
                Opcode::PopFrame => {
                    let ret = self.dynadata.frames_stack.pop();
                    if ret.is_none() {
                        return Err(RuntimeError::new(
                            Box::new(self.run_contnet.clone()),
                            ErrorInfo::new(gettext(VM_FRAME_EMPTY), gettext(VM_ERROR)),
                        ));
                    }
                }
                Opcode::LoadInt => {
                    self.dynadata.int_stack.push(
                        self.static_data.constpool.intpool[self.static_data.inst[self.pc].operand],
                    );
                }
                Opcode::BitAnd => operator_opcode!(bit_and, self),
                Opcode::BitOr => operator_opcode!(bit_or, self),
                Opcode::BitNot => operator_opcode!(bit_not, self),
                Opcode::BitLeftShift => operator_opcode!(bit_left_shift, self),
                Opcode::BitRightShift => operator_opcode!(bit_right_shift, self),
                Opcode::LoadLocal => {}
                Opcode::StoreLocal => {}
                Opcode::LoadString => {
                    self.dynadata.str_stack.push(
                        self.static_data.constpool.stringpool
                            [self.static_data.inst[self.pc].operand]
                            .clone(),
                    );
                }
                Opcode::LoadFloat => {
                    self.dynadata.float_stack.push(
                        self.static_data.constpool.floatpool
                            [self.static_data.inst[self.pc].operand],
                    );
                }
                Opcode::LoadBigInt => {}
                Opcode::Empty => {}
                Opcode::SelfNegative => {
                    operator_opcode!(self_negative, self);
                }
                Opcode::CallNative => unsafe {
                    let tmp =
                        STD_FUNC_TABLE[self.static_data.inst[self.pc].operand](&mut self.dynadata);
                    self.throw_err_info(tmp)?;
                },
                Opcode::AddInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.int_stack.push(first + second);
                }
                Opcode::AddFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata.float_stack.push(first + second);
                }
                Opcode::AddStr => {
                    let (first, second) = impl_opcode!(self.dynadata.str_stack, self, 2);
                    self.dynadata.str_stack.push(format!("{}{}", first, second));
                }
                Opcode::SubInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.int_stack.push(first - second);
                }
                Opcode::SubFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata.float_stack.push(first - second);
                }
                Opcode::MulInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.int_stack.push(first * second);
                }
                Opcode::MulFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata.float_stack.push(first * second);
                }
                Opcode::DivInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata
                        .float_stack
                        .push(self.throw_err_info(div_int(first, second))?);
                }
                Opcode::DivFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata
                        .float_stack
                        .push(self.throw_err_info(div_float(first, second))?);
                }
                Opcode::ExactDivInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata
                        .int_stack
                        .push(self.throw_err_info(exact_div_int(first, second))?);
                }
                Opcode::ExtraDivFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata
                        .int_stack
                        .push(self.throw_err_info(exact_div_float(first, second))?);
                }
                Opcode::ModInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata
                        .int_stack
                        .push(self.throw_err_info(mod_int(first, second))?);
                }
                Opcode::PowerInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.int_stack.push(power_int(first, second));
                }
                Opcode::EqInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.bool_stack.push(first == second);
                }
                Opcode::EqFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata.bool_stack.push(first == second);
                }
                Opcode::EqStr => {
                    let (first, second) = impl_opcode!(self.dynadata.str_stack, self, 2);
                    self.dynadata.bool_stack.push(first == second);
                }
                Opcode::EqChar => {
                    let (first, second) = impl_opcode!(self.dynadata.char_stack, self, 2);
                    self.dynadata.bool_stack.push(first == second);
                }
                Opcode::EqBool => {
                    let (first, second) = impl_opcode!(self.dynadata.bool_stack, self, 2);
                    self.dynadata.bool_stack.push(first == second);
                }
                Opcode::NeInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.bool_stack.push(first != second);
                }
                Opcode::NeFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata.bool_stack.push(first != second);
                }
                Opcode::NeStr => {
                    let (first, second) = impl_opcode!(self.dynadata.str_stack, self, 2);
                    self.dynadata.bool_stack.push(first != second);
                }
                Opcode::NeChar => {
                    let (first, second) = impl_opcode!(self.dynadata.char_stack, self, 2);
                    self.dynadata.bool_stack.push(first != second);
                }
                Opcode::NeBool => {
                    let (first, second) = impl_opcode!(self.dynadata.bool_stack, self, 2);
                    self.dynadata.bool_stack.push(first != second);
                }
                Opcode::LtInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.bool_stack.push(first < second);
                }
                Opcode::LtFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata.bool_stack.push(first < second);
                }
                Opcode::LeInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.bool_stack.push(first <= second);
                }
                Opcode::LeFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata.bool_stack.push(first <= second);
                }
                Opcode::GtInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.bool_stack.push(first > second);
                }
                Opcode::GtFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata.bool_stack.push(first > second);
                }
                Opcode::GeInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.bool_stack.push(first >= second);
                }
                Opcode::GeFloat => {
                    let (first, second) = impl_opcode!(self.dynadata.float_stack, self, 2);
                    self.dynadata.bool_stack.push(first >= second);
                }
                Opcode::AndBool => {
                    let (first, second) = impl_opcode!(self.dynadata.bool_stack, self, 2);
                    self.dynadata.bool_stack.push(first && second);
                }
                Opcode::OrBool => {
                    let (first, second) = impl_opcode!(&mut self.dynadata.bool_stack, self, 2);
                    self.dynadata.bool_stack.push(first || second);
                }
                Opcode::NotBool => {
                    let first = impl_opcode!(self.dynadata.bool_stack, self, 1);
                    self.dynadata.bool_stack.push(!first);
                }
                Opcode::XorInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.int_stack.push(first ^ second);
                }
                Opcode::BitNotInt => {
                    let first = impl_opcode!(self.dynadata.int_stack, self, 1);
                    self.dynadata.int_stack.push(!first);
                }
                Opcode::BitAndInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.int_stack.push(first & second);
                }
                Opcode::BitOrInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.int_stack.push(first | second);
                }
                Opcode::BitLeftShiftInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.int_stack.push(first << second);
                }
                Opcode::BitRightShiftInt => {
                    let (first, second) = impl_opcode!(self.dynadata.int_stack, self, 2);
                    self.dynadata.int_stack.push(first >> second);
                }
                Opcode::SelfNegativeInt => {
                    let first = impl_opcode!(self.dynadata.int_stack, self, 1);
                    self.dynadata.int_stack.push(-first);
                }
                Opcode::SelfNegativeFloat => {
                    let first = impl_opcode!(self.dynadata.float_stack, self, 1);
                    self.dynadata.float_stack.push(-first);
                }
                Opcode::JumpIfFalse => {
                    let condit = impl_opcode!(self.dynadata.bool_stack, self, 1);
                    if !condit {
                        self.pc = self.static_data.inst[self.pc].operand;
                    }
                }
                Opcode::Jump => {
                    self.pc = self.static_data.inst[self.pc].operand;
                }
                Opcode::LoadChar => unsafe {
                    self.dynadata.char_stack.push(char::from_u32_unchecked(
                        self.static_data.inst[self.pc].operand as u32,
                    ));
                },
                Opcode::LoadBool => {
                    self.dynadata
                        .bool_stack
                        .push(self.static_data.inst[self.pc].operand != 0);
                }
                Opcode::MoveInt => self.dynadata.obj_stack.push(Box::new(TrcInt::new(
                    self.dynadata.int_stack.pop().unwrap(),
                ))),
                Opcode::MoveFloat => self.dynadata.obj_stack.push(Box::new(TrcFloat::new(
                    self.dynadata.float_stack.pop().unwrap(),
                ))),
                Opcode::MoveChar => self.dynadata.obj_stack.push(Box::new(TrcChar::new(
                    self.dynadata.char_stack.pop().unwrap(),
                ))),
                Opcode::MoveBool => self.dynadata.obj_stack.push(Box::new(TrcBool::new(
                    self.dynadata.bool_stack.pop().unwrap(),
                ))),
                Opcode::MoveStr => self.dynadata.obj_stack.push(Box::new(TrcStr::new(
                    self.dynadata.str_stack.pop().unwrap(),
                ))),
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
