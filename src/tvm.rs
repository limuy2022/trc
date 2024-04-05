mod def;
mod function;

use self::function::Frame;
use crate::cfg;
use libcore::*;
use libloading::Library;
use rust_i18n::t;
use std::sync::OnceLock;

pub struct Vm<'a> {
    run_context: Context,
    dynadata: DydataWrap,
    static_data: &'a libcore::codegen::StaticData,
}

#[derive(Debug, Clone)]
struct Context {
    module_name: String,
    line_pos: usize,
}

impl ErrorContext for Context {
    fn get_module_name(&self) -> &str {
        &self.module_name
    }

    fn get_line(&self) -> usize {
        self.line_pos
    }
}

impl Context {
    fn new(module_name: &str) -> Self {
        Context {
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
    static LIBC: OnceLock<Option<Library>> = OnceLock::new();
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

pub struct DydataWrap {
    frames_stack: Vec<Frame>,
    dydata: DynaData,
}

impl DydataWrap {
    fn new() -> Self {
        Self {
            frames_stack: Vec::new(),
            dydata: DynaData::new(),
        }
    }
}

/// reduce the duplicate code to solve the operator running
macro_rules! operator_opcode {
    ($trait_used:ident, $sself:expr) => {{
        let ret = libcore::types::$trait_used(&mut $sself.dynadata.dydata);
        match ret {
            Err(e) => {
                return Err(libcore::error::RuntimeError::new(
                    Box::new($sself.run_context.clone()),
                    e,
                ));
            }
            Ok(_) => {}
        }
    }};
}

macro_rules! impl_opcode {
    ($obj_ty:path, $sself:expr, 2) => {{
        let second = $sself.dynadata.dydata.pop_data::<$obj_ty>();
        let first = $sself.dynadata.dydata.pop_data::<$obj_ty>();
        (first, second)
    }};
    ($obj_ty:path, $sself:expr, 1) => {{
        let first = $sself.dynadata.dydata.pop_data::<$obj_ty>();
        (first)
    }};
}

/// 生成不会pop数据的运算指令
macro_rules! impl_opcode_without_pop_first_data {
    ($obj_ty:path, $sself:expr) => {{
        let second = $sself.dynadata.dydata.pop_data::<$obj_ty>();
        let first = $sself.dynadata.dydata.read_top_data::<$obj_ty>();
        (first, second)
    }};
}

macro_rules! impl_store_local_var {
    ($ty:path, $pc:expr, $sself:expr) => {{
        let data = $sself.dynadata.dydata.pop_data::<$ty>();
        let addr = $sself.static_data.inst[$pc].operand;
        $sself
            .dynadata
            .frames_stack
            .last_mut()
            .unwrap()
            .set_var(addr, data)
    }};
}

macro_rules! impl_load_local_var {
    ($ty:path, $pc:expr, $sself:expr) => {
        let data = $sself
            .dynadata
            .frames_stack
            .last()
            .unwrap()
            .get_var::<$ty>($sself.static_data.inst[$pc].operand);
        $sself.dynadata.dydata.push_data(data);
    };
}

impl<'a> Vm<'a> {
    pub fn new(static_data: &'a StaticData) -> Self {
        Self {
            dynadata: DydataWrap::new(),
            run_context: Context::new(cfg::MAIN_MODULE_NAME),
            static_data,
        }
    }

    pub fn set_static_data(&mut self, static_data: &'a StaticData) {
        self.static_data = static_data;
    }

    fn throw_err_info<T>(&self, info: RuntimeResult<T>) -> RunResult<T> {
        match info {
            Ok(data) => Ok(data),
            Err(e) => Err(RuntimeError::new(Box::new(self.run_context.clone()), e)),
        }
    }

    pub fn reset(&mut self) {
        self.dynadata
            .dydata
            .init_global_var_store(self.static_data.sym_table_sz);
    }

    #[inline]
    fn run_opcode(&mut self, pc: &mut usize) -> Result<(), RuntimeError> {
        match self.static_data.inst[*pc].opcode {
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
            Opcode::PopFrame => {
                let ret = match self.dynadata.frames_stack.pop() {
                    None => {
                        return self
                            .throw_err_info(Err(ErrorInfo::new(t!(VM_FRAME_EMPTY), t!(VM_ERROR))))
                    }
                    Some(v) => v,
                };
                *pc = ret.prev_addr;
            }
            Opcode::LoadInt => {
                self.dynadata.dydata.push_data(
                    self.static_data.constpool.intpool[self.static_data.inst[*pc].operand],
                );
            }
            Opcode::BitAnd => operator_opcode!(bit_and, self),
            Opcode::BitOr => operator_opcode!(bit_or, self),
            Opcode::BitNot => operator_opcode!(bit_not, self),
            Opcode::BitLeftShift => operator_opcode!(bit_left_shift, self),
            Opcode::BitRightShift => operator_opcode!(bit_right_shift, self),
            Opcode::LoadLocalVarObj => {
                self.dynadata
                    .dydata
                    .push_data(self.dynadata.dydata.var_store[self.static_data.inst[*pc].operand]);
            }
            Opcode::StoreLocalObj => {
                self.dynadata.dydata.var_store[self.static_data.inst[*pc].operand] =
                    self.dynadata.dydata.pop_data();
            }
            Opcode::LoadString => {
                let tmp = self.static_data.inst[*pc].operand;
                let tmp = self.static_data.constpool.stringpool[tmp].clone();
                let tmp = self.dynadata.dydata.gc.alloc(tmp);
                self.dynadata.dydata.push_data(tmp);
            }
            Opcode::LoadFloat => {
                self.dynadata.dydata.push_data(
                    self.static_data.constpool.floatpool[self.static_data.inst[*pc].operand],
                );
            }
            Opcode::LoadBigInt => {}
            Opcode::Empty => {}
            Opcode::SelfNegative => {
                operator_opcode!(self_negative, self);
            }
            Opcode::CallNative => unsafe {
                // let tmp =
                //     STD_FUNC_TABLE[self.static_data.inst[*pc].operand](&mut self.dynadata.dydata);
                // self.throw_err_info(tmp)?;
            },
            Opcode::AddInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first + second);
            }
            Opcode::AddFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.dydata.push_data(first + second);
            }
            Opcode::AddStr => {
                let (first, second) = impl_opcode!(TrcStrInternal, self, 2);
                let tmp = self
                    .dynadata
                    .dydata
                    .gc
                    .alloc(unsafe { format!("{}{}", *first, *second) });
                self.dynadata.dydata.push_data(tmp);
            }
            Opcode::SubInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first - second);
            }
            Opcode::SubFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.dydata.push_data(first - second);
            }
            Opcode::MulInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first * second);
            }
            Opcode::MulFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.dydata.push_data(first * second);
            }
            Opcode::DivInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata
                    .dydata
                    .push_data(self.throw_err_info(div_int(first, second))?);
            }
            Opcode::DivFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata
                    .dydata
                    .push_data(self.throw_err_info(div_float(first, second))?);
            }
            Opcode::ExactDivInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata
                    .dydata
                    .push_data(self.throw_err_info(exact_div_int(first, second))?);
            }
            Opcode::ExtraDivFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata
                    .dydata
                    .push_data(self.throw_err_info(exact_div_float(first, second))?);
            }
            Opcode::ModInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata
                    .dydata
                    .push_data(self.throw_err_info(mod_int(first, second))?);
            }
            Opcode::PowerInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(power_int(first, second));
            }
            Opcode::EqInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first == second);
            }
            Opcode::EqFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.dydata.push_data(first == second);
            }
            Opcode::EqStr => {
                let (first, second) = impl_opcode!(TrcStrInternal, self, 2);
                self.dynadata.dydata.push_data(unsafe { *first == *second });
            }
            Opcode::EqChar => {
                let (first, second) = impl_opcode!(TrcCharInternal, self, 2);
                self.dynadata.dydata.push_data(first == second);
            }
            Opcode::EqBool => {
                let (first, second) = impl_opcode!(bool, self, 2);
                self.dynadata.dydata.push_data(first == second);
            }
            Opcode::NeInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first != second);
            }
            Opcode::NeFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.dydata.push_data(first != second);
            }
            Opcode::NeStr => {
                let (first, second) = impl_opcode!(TrcStrInternal, self, 2);
                self.dynadata.dydata.push_data(unsafe { *first != *second });
            }
            Opcode::NeChar => {
                let (first, second) = impl_opcode!(TrcCharInternal, self, 2);
                self.dynadata.dydata.push_data(first != second);
            }
            Opcode::NeBool => {
                let (first, second) = impl_opcode!(bool, self, 2);
                self.dynadata.dydata.push_data(first != second);
            }
            Opcode::LtInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first < second);
            }
            Opcode::LtFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.dydata.push_data(first < second);
            }
            Opcode::LeInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first <= second);
            }
            Opcode::LeFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.dydata.push_data(first <= second);
            }
            Opcode::GtInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first > second);
            }
            Opcode::GtFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.dydata.push_data(first > second);
            }
            Opcode::GeInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first >= second);
            }
            Opcode::GeFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.dydata.push_data(first >= second);
            }
            Opcode::AndBool => {
                let (first, second) = impl_opcode!(bool, self, 2);
                self.dynadata.dydata.push_data(first && second);
            }
            Opcode::OrBool => {
                let (first, second) = impl_opcode!(bool, self, 2);
                self.dynadata.dydata.push_data(first || second);
            }
            Opcode::NotBool => {
                let first = impl_opcode!(bool, self, 1);
                self.dynadata.dydata.push_data(!first);
            }
            Opcode::XorInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first ^ second);
            }
            Opcode::BitNotInt => {
                let first = impl_opcode!(TrcIntInternal, self, 1);
                self.dynadata.dydata.push_data(!first);
            }
            Opcode::BitAndInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first & second);
            }
            Opcode::BitOrInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first | second);
            }
            Opcode::BitLeftShiftInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first << second);
            }
            Opcode::BitRightShiftInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.dydata.push_data(first >> second);
            }
            Opcode::SelfNegativeInt => {
                let first = impl_opcode!(TrcIntInternal, self, 1);
                self.dynadata.dydata.push_data(-first);
            }
            Opcode::SelfNegativeFloat => {
                let first = impl_opcode!(TrcFloatInternal, self, 1);
                self.dynadata.dydata.push_data(-first);
            }
            Opcode::JumpIfFalse => {
                let condit = impl_opcode!(bool, self, 1);
                if !condit {
                    *pc = self.static_data.inst[*pc].operand;
                    return Ok(());
                }
            }
            Opcode::Jump => {
                *pc = self.static_data.inst[*pc].operand;
                return Ok(());
            }
            Opcode::LoadChar => unsafe {
                self.dynadata.dydata.push_data(char::from_u32_unchecked(
                    self.static_data.inst[*pc].operand as u32,
                ));
            },
            Opcode::LoadBool => {
                self.dynadata
                    .dydata
                    .push_data(self.static_data.inst[*pc].operand != 0);
            }
            Opcode::MoveInt => {
                let tmp = TrcInt::new(self.dynadata.dydata.pop_data());
                let ptr = self.dynadata.dydata.gc.alloc(tmp);
                self.dynadata.dydata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::MoveFloat => {
                let tmp = TrcFloat::new(self.dynadata.dydata.pop_data());
                let ptr = self.dynadata.dydata.gc.alloc(tmp);
                self.dynadata.dydata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::MoveChar => {
                let tmp = TrcChar::new(self.dynadata.dydata.pop_data());
                let ptr = self.dynadata.dydata.gc.alloc(tmp);
                self.dynadata.dydata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::MoveBool => {
                let tmp = TrcBool::new(self.dynadata.dydata.pop_data());
                let ptr = self.dynadata.dydata.gc.alloc(tmp);
                self.dynadata.dydata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::MoveStr => {
                // todo:inmprove performance
                let tmp = TrcStr::new(self.dynadata.dydata.pop_data());
                let ptr = self.dynadata.dydata.gc.alloc(tmp);
                self.dynadata.dydata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::StoreLocalInt => {
                impl_store_local_var!(TrcIntInternal, *pc, self);
            }
            Opcode::StoreLocalFloat => {
                impl_store_local_var!(TrcFloatInternal, *pc, self);
            }
            Opcode::StoreLocalChar => {
                impl_store_local_var!(TrcCharInternal, *pc, self);
            }
            Opcode::StoreLocalBool => {
                impl_store_local_var!(bool, *pc, self);
            }
            Opcode::StoreLocalStr => {
                impl_store_local_var!(TrcStrInternal, *pc, self);
            }
            Opcode::LoadLocalVarBool => {
                impl_load_local_var!(bool, *pc, self);
            }
            Opcode::LoadLocalVarInt => {
                impl_load_local_var!(TrcIntInternal, *pc, self);
            }
            Opcode::LoadLocalVarFloat => {
                impl_load_local_var!(TrcFloatInternal, *pc, self);
            }
            Opcode::LoadLocalVarStr => {
                impl_load_local_var!(TrcStrInternal, *pc, self);
            }
            Opcode::LoadLocalVarChar => {
                impl_load_local_var!(TrcCharInternal, *pc, self);
            }
            Opcode::Stop => {
                *pc = self.static_data.inst.len();
                return Ok(());
            }
            Opcode::CallCustom => {
                let var_table_mem_sz =
                    self.static_data.funcs[self.static_data.inst[*pc].operand].var_table_sz;
                let space = self.dynadata.dydata.alloc_var_space(var_table_mem_sz);
                self.dynadata.frames_stack.push(Frame::new(*pc, space));
                *pc = self.static_data.funcs[self.static_data.inst[*pc].operand].func_addr;
                return Ok(());
            }
            Opcode::StoreGlobalObj => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.pop_data::<*mut dyn TrcObj>();
                self.dynadata.dydata.set_var(addr, data);
            }
            Opcode::StoreGlobalInt => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.pop_data::<TrcIntInternal>();
                self.dynadata.dydata.set_var(addr, data);
            }
            Opcode::StoreGlobalFloat => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.pop_data::<TrcFloatInternal>();
                self.dynadata.dydata.set_var(addr, data);
            }
            Opcode::StoreGlobalChar => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.pop_data::<TrcCharInternal>();
                self.dynadata.dydata.set_var(addr, data);
            }
            Opcode::StoreGlobalBool => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.pop_data::<bool>();
                self.dynadata.dydata.set_var(addr, data);
            }
            Opcode::StoreGlobalStr => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.pop_data::<TrcStrInternal>();
                self.dynadata.dydata.set_var(addr, data);
            }
            Opcode::LoadGlobalVarObj => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.get_var::<*mut dyn TrcObj>(addr);
                self.dynadata.dydata.push_data(data);
            }
            Opcode::LoadGlobalVarBool => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.get_var::<bool>(addr);
                self.dynadata.dydata.push_data(data);
            }
            Opcode::LoadGlobalVarInt => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.get_var::<TrcIntInternal>(addr);
                self.dynadata.dydata.push_data(data);
            }
            Opcode::LoadGlobalVarFloat => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.get_var::<TrcFloatInternal>(addr);
                self.dynadata.dydata.push_data(data);
            }
            Opcode::LoadGlobalVarStr => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.get_var::<TrcStrInternal>(addr);
                self.dynadata.dydata.push_data(data);
            }
            Opcode::LoadGlobalVarChar => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.dydata.get_var::<TrcCharInternal>(addr);
                self.dynadata.dydata.push_data(data);
            }
            Opcode::EqWithoutPop => {
                // let (first, second) = impl_opcode_without_pop_first_data!(*mut dyn TrcObj, self);
                // self.dynadata.dydata.push_data(first == second);
            }
            Opcode::EqIntWithoutPop => {
                let (first, second) = impl_opcode_without_pop_first_data!(TrcIntInternal, self);
                self.dynadata.dydata.push_data(first == second);
            }
            Opcode::EqFloatWithoutPop => {
                let (first, second) = impl_opcode_without_pop_first_data!(TrcFloatInternal, self);
                self.dynadata.dydata.push_data(first == second);
            }
            Opcode::EqStrWithoutPop => {
                let (first, second) = impl_opcode_without_pop_first_data!(TrcStrInternal, self);
                self.dynadata.dydata.push_data(unsafe { *first == *second });
            }
            Opcode::EqCharWithoutPop => {
                let (first, second) = impl_opcode_without_pop_first_data!(TrcCharInternal, self);
                self.dynadata.dydata.push_data(first == second);
            }
            Opcode::EqBoolWithoutPop => {
                let (first, second) = impl_opcode_without_pop_first_data!(bool, self);
                self.dynadata.dydata.push_data(first == second);
            }
            Opcode::PopData => {
                self.dynadata.dydata.pop_data::<*mut dyn TrcObj>();
            }
            Opcode::PopDataInt => {
                self.dynadata.dydata.pop_data::<TrcIntInternal>();
            }
            Opcode::PopDataFloat => {
                self.dynadata.dydata.pop_data::<TrcFloatInternal>();
            }
            Opcode::PopDataStr => {
                self.dynadata.dydata.pop_data::<TrcStrInternal>();
            }
            Opcode::PopDataChar => {
                self.dynadata.dydata.pop_data::<TrcCharInternal>();
            }
            Opcode::PopDataBool => {
                self.dynadata.dydata.pop_data::<bool>();
            }
            Opcode::JumpIfTrue => {
                let condit = impl_opcode!(bool, self, 1);
                if condit {
                    *pc = self.static_data.inst[*pc].operand;
                    return Ok(());
                }
            }
            Opcode::ImportNativeModule => todo!(),
        };
        *pc += 1;
        Ok(())
    }

    pub fn run(&mut self) -> Result<(), RuntimeError> {
        self.reset();
        let mut pc = 0;
        if !self.static_data.line_table.is_empty() {
            while pc < self.static_data.inst.len() {
                self.run_context.set_line(self.static_data.line_table[pc]);
                self.run_opcode(&mut pc)?;
            }
        } else {
            while pc < self.static_data.inst.len() {
                self.run_opcode(&mut pc)?;
            }
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {}
