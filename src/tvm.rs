mod algo;
mod def;
mod function;
mod gc;
pub mod stdlib;
mod types;

use core::panic;
use std::{any::TypeId, mem::size_of, sync::OnceLock};

use self::{
    function::Frame,
    gc::GcMgr,
    types::{
        trcchar::TrcCharInternal,
        trcfloat::{div_float, exact_div_float, TrcFloat, TrcFloatInternal},
        trcint::{div_int, exact_div_int, mod_int, power_int, TrcInt, TrcIntInternal},
        trcstr::{TrcStr, TrcStrInternal},
        TrcBool, TrcChar, TrcObj,
    },
};
use crate::{
    base::{
        codegen::{Opcode, StaticData},
        error::*,
        stdlib::STD_FUNC_TABLE,
    },
    cfg,
};
use libloading::Library;
use rust_i18n::t;

pub fn get_max_stack_sz() -> usize {
    static T: OnceLock<usize> = OnceLock::new();
    *T.get_or_init(|| 1024 * 1024 * 2 / size_of::<Byte>())
}

pub fn get_trcobj_sz() -> usize {
    static T: OnceLock<usize> = OnceLock::new();
    *T.get_or_init(size_of::<*mut dyn TrcObj>)
}

type Byte = u64;

#[derive(Default)]
pub struct DynaData {
    gc: GcMgr,
    run_stack: Vec<Byte>,
    frames_stack: Vec<Frame>,
    var_store: Vec<Byte>,
    stack_ptr: usize,
    // 变量已经使用的内存空间大小
    var_used: usize,
    #[cfg(debug_assertions)]
    type_used: Vec<(TypeId, &'static str)>,
}

impl DynaData {
    pub fn new() -> Self {
        let mut ret = Self {
            run_stack: Vec::with_capacity(get_max_stack_sz()),
            var_store: Vec::with_capacity(get_max_stack_sz()),
            stack_ptr: 0,
            ..Default::default()
        };
        unsafe {
            ret.run_stack.set_len(get_max_stack_sz());
            ret.var_store.set_len(get_max_stack_sz());
        }
        ret
    }

    pub fn init_global_var_store(&mut self, cap: usize) {
        self.var_used = cap;
        if self.var_store.len() > cap {
            return;
        }
        self.var_store.resize(cap, Byte::default());
    }

    pub fn push_data<T: 'static>(&mut self, data: T) {
        unsafe {
            (self
                .run_stack
                .as_mut_ptr()
                .byte_offset(self.stack_ptr as isize) as *mut T)
                .write(data);
        }
        self.stack_ptr += size_of::<T>();
        #[cfg(debug_assertions)]
        {
            self.type_used
                .push((TypeId::of::<T>(), std::any::type_name::<T>()));
        }
    }

    pub fn pop_data<T: Copy + 'static>(&mut self) -> T {
        let sz = size_of::<T>();
        #[cfg(debug_assertions)]
        {
            let info = TypeId::of::<T>();
            let info_stack = self.type_used.pop().unwrap();
            if info_stack.0 != info {
                panic!(
                    "pop data type error.Expected get {}.Actually has {}",
                    std::any::type_name::<T>(),
                    info_stack.1
                );
            }
            debug_assert!(self.stack_ptr >= sz);
        }
        self.stack_ptr -= sz;
        unsafe { *(self.run_stack.as_ptr().byte_offset(self.stack_ptr as isize) as *const T) }
    }

    pub fn set_var<T: 'static>(&mut self, addr: usize, data: T) {
        unsafe {
            *(self.var_store.as_mut_ptr().byte_offset(addr as isize) as *mut T) = data;
        }
    }

    pub fn get_var<T: Copy + 'static>(&self, addr: usize) -> T {
        debug_assert!(addr < self.var_used);
        unsafe { *(self.var_store.as_ptr().byte_offset(addr as isize) as *const T) }
    }

    pub fn alloc_var_space(&mut self, need_sz: usize) -> *mut Byte {
        self.var_used += need_sz;
        if self.var_used > self.var_store.len() {
            self.var_store.resize(self.var_used, Byte::default());
        }
        unsafe {
            self.var_store
                .as_mut_ptr()
                .byte_offset((self.var_used - need_sz) as isize)
        }
    }

    pub fn dealloc_var_space(&mut self, need_sz: usize) {
        self.var_used -= need_sz;
    }
}

pub struct Vm<'a> {
    run_context: Context,
    dynadata: DynaData,
    static_data: &'a StaticData,
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

/// reduce the duplicate code to solve the operator running
macro_rules! operator_opcode {
    ($trait_used:ident, $sself:expr) => {{
        let ret = types::$trait_used(&mut $sself.dynadata);
        match ret {
            Err(e) => {
                return Err(RuntimeError::new(Box::new($sself.run_context.clone()), e));
            }
            Ok(_) => {}
        }
    }};
}
macro_rules! impl_opcode {
    ($obj_ty:path, $sself:expr, 2) => {{
        let second = $sself.dynadata.pop_data::<$obj_ty>();
        let first = $sself.dynadata.pop_data::<$obj_ty>();
        (first, second)
    }};
    ($obj_ty:path, $sself:expr, 1) => {{
        let first = $sself.dynadata.pop_data::<$obj_ty>();
        (first)
    }};
}

impl<'a> Vm<'a> {
    pub fn new(static_data: &'a StaticData) -> Self {
        Self {
            dynadata: DynaData::new(),
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
                self.dynadata.push_data(
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
                    .push_data(self.dynadata.var_store[self.static_data.inst[*pc].operand]);
            }
            Opcode::StoreLocalObj => {
                self.dynadata.var_store[self.static_data.inst[*pc].operand] =
                    self.dynadata.pop_data();
            }
            Opcode::LoadString => {
                let tmp = self.static_data.inst[*pc].operand;
                let tmp = self.static_data.constpool.stringpool[tmp].clone();
                let tmp = self.dynadata.gc.alloc(tmp);
                self.dynadata.push_data(tmp);
            }
            Opcode::LoadFloat => {
                self.dynadata.push_data(
                    self.static_data.constpool.floatpool[self.static_data.inst[*pc].operand],
                );
            }
            Opcode::LoadBigInt => {}
            Opcode::Empty => {}
            Opcode::SelfNegative => {
                operator_opcode!(self_negative, self);
            }
            Opcode::CallNative => unsafe {
                let tmp = STD_FUNC_TABLE[self.static_data.inst[*pc].operand](&mut self.dynadata);
                self.throw_err_info(tmp)?;
            },
            Opcode::AddInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first + second);
            }
            Opcode::AddFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.push_data(first + second);
            }
            Opcode::AddStr => {
                let (first, second) = impl_opcode!(TrcStrInternal, self, 2);
                let tmp = self
                    .dynadata
                    .gc
                    .alloc(unsafe { format!("{}{}", *first, *second) });
                self.dynadata.push_data(tmp);
            }
            Opcode::SubInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first - second);
            }
            Opcode::SubFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.push_data(first - second);
            }
            Opcode::MulInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first * second);
            }
            Opcode::MulFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.push_data(first * second);
            }
            Opcode::DivInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata
                    .push_data(self.throw_err_info(div_int(first, second))?);
            }
            Opcode::DivFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata
                    .push_data(self.throw_err_info(div_float(first, second))?);
            }
            Opcode::ExactDivInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata
                    .push_data(self.throw_err_info(exact_div_int(first, second))?);
            }
            Opcode::ExtraDivFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata
                    .push_data(self.throw_err_info(exact_div_float(first, second))?);
            }
            Opcode::ModInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata
                    .push_data(self.throw_err_info(mod_int(first, second))?);
            }
            Opcode::PowerInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(power_int(first, second));
            }
            Opcode::EqInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first == second);
            }
            Opcode::EqFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.push_data(first == second);
            }
            Opcode::EqStr => {
                let (first, second) = impl_opcode!(TrcStrInternal, self, 2);
                self.dynadata.push_data(first == second);
            }
            Opcode::EqChar => {
                let (first, second) = impl_opcode!(TrcCharInternal, self, 2);
                self.dynadata.push_data(first == second);
            }
            Opcode::EqBool => {
                let (first, second) = impl_opcode!(bool, self, 2);
                self.dynadata.push_data(first == second);
            }
            Opcode::NeInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first != second);
            }
            Opcode::NeFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.push_data(first != second);
            }
            Opcode::NeStr => {
                let (first, second) = impl_opcode!(TrcStrInternal, self, 2);
                self.dynadata.push_data(first != second);
            }
            Opcode::NeChar => {
                let (first, second) = impl_opcode!(TrcCharInternal, self, 2);
                self.dynadata.push_data(first != second);
            }
            Opcode::NeBool => {
                let (first, second) = impl_opcode!(bool, self, 2);
                self.dynadata.push_data(first != second);
            }
            Opcode::LtInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first < second);
            }
            Opcode::LtFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.push_data(first < second);
            }
            Opcode::LeInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first <= second);
            }
            Opcode::LeFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.push_data(first <= second);
            }
            Opcode::GtInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first > second);
            }
            Opcode::GtFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.push_data(first > second);
            }
            Opcode::GeInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first >= second);
            }
            Opcode::GeFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata.push_data(first >= second);
            }
            Opcode::AndBool => {
                let (first, second) = impl_opcode!(bool, self, 2);
                self.dynadata.push_data(first && second);
            }
            Opcode::OrBool => {
                let (first, second) = impl_opcode!(bool, self, 2);
                self.dynadata.push_data(first || second);
            }
            Opcode::NotBool => {
                let first = impl_opcode!(bool, self, 1);
                self.dynadata.push_data(!first);
            }
            Opcode::XorInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first ^ second);
            }
            Opcode::BitNotInt => {
                let first = impl_opcode!(TrcIntInternal, self, 1);
                self.dynadata.push_data(!first);
            }
            Opcode::BitAndInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first & second);
            }
            Opcode::BitOrInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first | second);
            }
            Opcode::BitLeftShiftInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first << second);
            }
            Opcode::BitRightShiftInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata.push_data(first >> second);
            }
            Opcode::SelfNegativeInt => {
                let first = impl_opcode!(TrcIntInternal, self, 1);
                self.dynadata.push_data(-first);
            }
            Opcode::SelfNegativeFloat => {
                let first = impl_opcode!(TrcFloatInternal, self, 1);
                self.dynadata.push_data(-first);
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
                self.dynadata.push_data(char::from_u32_unchecked(
                    self.static_data.inst[*pc].operand as u32,
                ));
            },
            Opcode::LoadBool => {
                self.dynadata
                    .push_data(self.static_data.inst[*pc].operand != 0);
            }
            Opcode::MoveInt => {
                let tmp = TrcInt::new(self.dynadata.pop_data());
                let ptr = self.dynadata.gc.alloc(tmp);
                self.dynadata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::MoveFloat => {
                let tmp = TrcFloat::new(self.dynadata.pop_data());
                let ptr = self.dynadata.gc.alloc(tmp);
                self.dynadata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::MoveChar => {
                let tmp = TrcChar::new(self.dynadata.pop_data());
                let ptr = self.dynadata.gc.alloc(tmp);
                self.dynadata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::MoveBool => {
                let tmp = TrcBool::new(self.dynadata.pop_data());
                let ptr = self.dynadata.gc.alloc(tmp);
                self.dynadata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::MoveStr => {
                // todo:inmprove performance
                let tmp = TrcStr::new(self.dynadata.pop_data());
                let ptr = self.dynadata.gc.alloc(tmp);
                self.dynadata.push_data(ptr as *mut dyn TrcObj);
            }
            Opcode::StoreLocalInt => {
                let data = self.dynadata.pop_data::<TrcIntInternal>();
                let addr = self.static_data.inst[*pc].operand;
                self.dynadata
                    .frames_stack
                    .last_mut()
                    .unwrap()
                    .set_var(addr, data)
            }
            Opcode::StoreLocalFloat => {
                let data = self.dynadata.pop_data::<TrcFloatInternal>();
                let addr = self.static_data.inst[*pc].operand;
                self.dynadata
                    .frames_stack
                    .last_mut()
                    .unwrap()
                    .set_var(addr, data)
            }
            Opcode::StoreLocalChar => {
                let data = self.dynadata.pop_data::<TrcCharInternal>();
                let addr = self.static_data.inst[*pc].operand;
                self.dynadata
                    .frames_stack
                    .last_mut()
                    .unwrap()
                    .set_var(addr, data)
            }
            Opcode::StoreLocalBool => {
                let data = self.dynadata.pop_data::<bool>();
                let addr = self.static_data.inst[*pc].operand;
                self.dynadata
                    .frames_stack
                    .last_mut()
                    .unwrap()
                    .set_var(addr, data)
            }
            Opcode::StoreLocalStr => {
                let data = self.dynadata.pop_data::<TrcStrInternal>();
                let addr = self.static_data.inst[*pc].operand;
                self.dynadata
                    .frames_stack
                    .last_mut()
                    .unwrap()
                    .set_var(addr, data)
            }
            Opcode::LoadLocalVarBool => {
                let data = self
                    .dynadata
                    .frames_stack
                    .last()
                    .unwrap()
                    .get_var::<bool>(self.static_data.inst[*pc].operand);
                self.dynadata.push_data(data);
            }
            Opcode::LoadLocalVarInt => {
                let data = self
                    .dynadata
                    .frames_stack
                    .last()
                    .unwrap()
                    .get_var::<TrcIntInternal>(self.static_data.inst[*pc].operand);
                self.dynadata.push_data(data);
            }
            Opcode::LoadLocalVarFloat => {
                let data = self
                    .dynadata
                    .frames_stack
                    .last()
                    .unwrap()
                    .get_var::<TrcFloatInternal>(self.static_data.inst[*pc].operand);
                self.dynadata.push_data(data);
            }
            Opcode::LoadLocalVarStr => {
                let data = self
                    .dynadata
                    .frames_stack
                    .last()
                    .unwrap()
                    .get_var::<TrcStrInternal>(self.static_data.inst[*pc].operand);
                self.dynadata.push_data(data);
            }
            Opcode::LoadLocalVarChar => {
                let data = self
                    .dynadata
                    .frames_stack
                    .last()
                    .unwrap()
                    .get_var::<TrcCharInternal>(self.static_data.inst[*pc].operand);
                self.dynadata.push_data(data);
            }
            Opcode::Stop => {
                *pc = self.static_data.inst.len();
                return Ok(());
            }
            Opcode::CallCustom => {
                let var_table_mem_sz =
                    self.static_data.funcs[self.static_data.inst[*pc].operand].var_table_sz;
                let space = self.dynadata.alloc_var_space(var_table_mem_sz);
                self.dynadata.frames_stack.push(Frame::new(*pc, space));
                *pc = self.static_data.funcs[self.static_data.inst[*pc].operand].func_addr;
                return Ok(());
            }
            Opcode::StoreGlobalObj => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.pop_data::<*mut dyn TrcObj>();
                self.dynadata.set_var(addr, data);
            }
            Opcode::StoreGlobalInt => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.pop_data::<TrcIntInternal>();
                self.dynadata.set_var(addr, data);
            }
            Opcode::StoreGlobalFloat => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.pop_data::<TrcFloatInternal>();
                self.dynadata.set_var(addr, data);
            }
            Opcode::StoreGlobalChar => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.pop_data::<TrcCharInternal>();
                self.dynadata.set_var(addr, data);
            }
            Opcode::StoreGlobalBool => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.pop_data::<bool>();
                self.dynadata.set_var(addr, data);
            }
            Opcode::StoreGlobalStr => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.pop_data::<TrcStrInternal>();
                self.dynadata.set_var(addr, data);
            }
            Opcode::LoadGlobalVarObj => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.get_var::<*mut dyn TrcObj>(addr);
                self.dynadata.push_data(data);
            }
            Opcode::LoadGlobalVarBool => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.get_var::<bool>(addr);
                self.dynadata.push_data(data);
            }
            Opcode::LoadGlobalVarInt => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.get_var::<TrcIntInternal>(addr);
                self.dynadata.push_data(data);
            }
            Opcode::LoadGlobalVarFloat => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.get_var::<TrcFloatInternal>(addr);
                self.dynadata.push_data(data);
            }
            Opcode::LoadGlobalVarStr => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.get_var::<TrcStrInternal>(addr);
                self.dynadata.push_data(data);
            }
            Opcode::LoadGlobalVarChar => {
                let addr = self.static_data.inst[*pc].operand;
                let data = self.dynadata.get_var::<TrcCharInternal>(addr);
                self.dynadata.push_data(data);
            }
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
mod tests {
    use super::*;
    use crate::compiler::*;
    use std::ptr::null_mut;

    #[test]
    fn test_dyna_data() {
        // panic!("deojfoejfoe");
        let mut data = DynaData::new();
        data.alloc_var_space(10);
        assert_eq!(data.var_used, 10);
        data.dealloc_var_space(10);
        assert_eq!(data.var_used, 0);
        data.push_data(10i64);
        data.push_data(20.0f64);
        data.push_data(30i64);
        data.push_data(null_mut::<i32>());
        data.push_data(50);
        data.push_data(false);
        assert_eq!(data.pop_data::<bool>(), false);
        assert_eq!(data.pop_data::<i32>(), 50);
        assert_eq!(data.pop_data::<*mut i32>(), null_mut::<i32>());
        assert_eq!(data.pop_data::<i64>(), 30);
        assert_eq!(data.pop_data::<f64>(), 20.0);
        assert_eq!(data.pop_data::<i64>(), 10);
    }

    macro_rules! gen_test_env {
        ($code:expr, $var:ident) => {
            let mut compiler = Compiler::new_string_compiler(
                Option::new(false, InputSource::StringInternal),
                $code,
            );
            let com_tmp = compiler.lex().unwrap();
            // println!("{:?}", com_tmp.inst);
            let tmp = com_tmp.return_static_data();
            let mut $var = Vm::new(&tmp);
        };
    }

    #[test]
    fn test_stdfunc() {
        gen_test_env!(r#"print("{},{},{},{}", 1, "h", 'p', true)"#, vm);
        vm.run().unwrap()
    }

    #[test]
    fn test_var_define() {
        gen_test_env!(
            r#"a:=10
        a=10
        print("{}", a)"#,
            vm
        );
        vm.run().unwrap()
    }

    #[test]
    fn test_if_easy() {
        gen_test_env!(r#"if 1==1 { println("ok") }"#, vm);
        vm.run().unwrap()
    }

    #[test]
    fn test_if_easy2() {
        gen_test_env!(r#"if 1==1 { println("ok") } else { println("error") }"#, vm);
        vm.run().unwrap()
    }

    #[test]
    fn test_if_final() {
        gen_test_env!(
            r#"a:=90
if a==90 {
  println("i equal to 90")
}
if a < 89 {
  println("i less than 90")
} else {
  if a % 2 == 0 {
    println("i is even")
  } else {
    println("i is odd")
  }
}"#,
            vm
        );
        vm.run().unwrap()
    }

    #[test]
    fn test_while() {
        gen_test_env!(
            r#"a:=10
while a > 0 {
    print("{}", a)
    a = a - 1
}"#,
            vm
        );
        vm.run().unwrap()
    }

    #[test]
    fn test_for() {
        gen_test_env!(
            r#"for i := 0; i <= 10; i=i+1 {
    print("{}", i)
}"#,
            vm
        );
        vm.run().unwrap()
    }

    #[test]
    fn test_call_custom_function() {
        gen_test_env!(
            r#"
func t() {
    print("90")
}
t()
t()
        "#,
            vm
        );
        vm.run().unwrap()
    }

    #[test]
    fn test_call_custom_function2() {
        gen_test_env!(
            r#"func tt() {
  println("I am function2")
}
func t() {
  println("I am function1")
  tt()
}
t()"#,
            vm
        );
        vm.run().unwrap()
    }
}
