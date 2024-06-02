mod def;
mod function;

use std::io::Write;

use self::function::Frame;
use crate::cfg;
use libcore::*;
use rust_i18n::t;

pub struct Vm<'a> {
    run_context: Context,
    dynadata: DydataWrap,
    static_data: &'a StaticData,
    imported_modules: Vec<(String, libloading::Library)>,
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

pub struct DydataWrap {
    frames_stack: Vec<Frame>,
    dydata: DynaData,
    imported_func: Vec<RustlibFunc>,
}

impl DydataWrap {
    fn new() -> Self {
        Self {
            frames_stack: Vec::new(),
            dydata: DynaData::new(),
            imported_func: Vec::new(),
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

impl<'a> Vm<'a> {
    pub fn new(static_data: &'a StaticData) -> Self {
        Self {
            dynadata: DydataWrap::new(),
            run_context: Context::new(cfg::MAIN_MODULE_NAME),
            static_data,
            imported_modules: Vec::new(),
        }
    }

    pub fn set_static_data(&mut self, static_data: &'a StaticData) {
        self.static_data = static_data;
    }

    fn convert_err_info<T>(&self, info: ErrorInfoResult<T>) -> RuntimeResult<T> {
        match info {
            Ok(data) => Ok(data),
            Err(e) => self.report_err(e),
        }
    }

    fn report_err<T>(&self, info: ErrorInfo) -> RuntimeResult<T> {
        Err(RuntimeError::new(Box::new(self.run_context.clone()), info))
    }

    pub fn reset(&mut self) -> RuntimeResult<()> {
        self.dynadata
            .dydata
            .init_global_var_store(self.static_data.global_sym_table_sz);
        let mut should_be_reloaded = false;
        for (i, j) in self.static_data.dll_module_should_loaded.iter().enumerate() {
            if i >= self.imported_modules.len() {
                break;
            }
            if *j != self.imported_modules[i].0 {
                should_be_reloaded = true;
                break;
            }
        }
        // 导入未导入的模块
        if !should_be_reloaded {
            for i in self
                .static_data
                .dll_module_should_loaded
                .iter()
                .skip(self.imported_modules.len())
            {
                let lib = self.import_module(i.clone())?;
                self.imported_modules.push((i.clone(), lib));
            }
        } else {
            self.imported_modules.clear();
            for i in &self.static_data.dll_module_should_loaded {
                let lib = self.import_module(i.clone())?;
                self.imported_modules.push((i.clone(), lib));
            }
        }
        Ok(())
    }

    #[inline]
    fn run_opcode(&mut self, pc: &mut usize) -> Result<(), RuntimeError> {
        // println!("run opcode:{}", self.static_data.inst[*pc].opcode);
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
                        return self.convert_err_info(Err(ErrorInfo::new(
                            t!(VM_FRAME_EMPTY),
                            t!(VM_ERROR),
                        )))
                    }
                    Some(v) => v,
                };
                *pc = ret.prev_addr;
            }
            Opcode::LoadInt => {
                self.dynadata.dydata.push_data(unsafe {
                    convert_to_int_constval(self.static_data.inst[*pc].operand.0)
                });
            }
            Opcode::BitAnd => operator_opcode!(bit_and, self),
            Opcode::BitOr => operator_opcode!(bit_or, self),
            Opcode::BitNot => operator_opcode!(bit_not, self),
            Opcode::BitLeftShift => operator_opcode!(bit_left_shift, self),
            Opcode::BitRightShift => operator_opcode!(bit_right_shift, self),
            Opcode::LoadLocalVar => {
                let addr = self.static_data.inst[*pc].operand.0;
                let bytes_num = self.static_data.inst[*pc].operand.1;
                let var_begin_addr = self
                    .dynadata
                    .frames_stack
                    .last()
                    .unwrap()
                    .get_addr(addr.into());
                unsafe {
                    self.dynadata
                        .dydata
                        .write_to_stack(var_begin_addr as *mut Byte, bytes_num.into())
                };
            }
            Opcode::LoadString => {
                let tmp = self.static_data.inst[*pc].operand.0;
                let idx: usize = tmp.into();
                let tmp = self.static_data.constpool.stringpool[idx].clone();
                let tmp = self.dynadata.dydata.gc.alloc(tmp);
                self.dynadata.dydata.push_data(tmp);
            }
            Opcode::LoadFloat => {
                let idx: usize = self.static_data.inst[*pc].operand.0.into();
                self.dynadata
                    .dydata
                    .push_data(self.static_data.constpool.floatpool[idx]);
            }
            Opcode::LoadBigInt => {}
            Opcode::Empty => {}
            Opcode::SelfNegative => {
                operator_opcode!(self_negative, self);
            }
            Opcode::CallNative => {
                let idx: usize = self.static_data.inst[*pc].operand.0.into();
                let tmp = self.dynadata.imported_func[idx](&mut self.dynadata.dydata);
                self.convert_err_info(tmp)?;
            }
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
                    .push_data(self.convert_err_info(div_int(first, second))?);
            }
            Opcode::DivFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata
                    .dydata
                    .push_data(self.convert_err_info(div_float(first, second))?);
            }
            Opcode::ExactDivInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata
                    .dydata
                    .push_data(self.convert_err_info(exact_div_int(first, second))?);
            }
            Opcode::ExtraDivFloat => {
                let (first, second) = impl_opcode!(TrcFloatInternal, self, 2);
                self.dynadata
                    .dydata
                    .push_data(self.convert_err_info(exact_div_float(first, second))?);
            }
            Opcode::ModInt => {
                let (first, second) = impl_opcode!(TrcIntInternal, self, 2);
                self.dynadata
                    .dydata
                    .push_data(self.convert_err_info(mod_int(first, second))?);
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
                    *pc = self.static_data.inst[*pc].operand.0.into();
                    return Ok(());
                }
            }
            Opcode::Jump => {
                *pc = self.static_data.inst[*pc].operand.0.into();
                // *pc += 1;
                return Ok(());
            }
            Opcode::LoadChar => unsafe {
                self.dynadata.dydata.push_data(char::from_u32_unchecked(
                    *self.static_data.inst[*pc].operand.0 as u32,
                ));
            },
            Opcode::LoadBool => {
                self.dynadata
                    .dydata
                    .push_data(*self.static_data.inst[*pc].operand.0 != 0);
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
            Opcode::Stop => {
                *pc = self.static_data.inst.len();
                return Ok(());
            }
            Opcode::CallCustom => {
                let idx: usize = self.static_data.inst[*pc].operand.0.into();
                let var_table_mem_sz = self.static_data.funcs_pos[idx].var_table_sz;
                let space = self.dynadata.dydata.alloc_var_space(var_table_mem_sz);
                self.dynadata.frames_stack.push(Frame::new(*pc, space));
                let idx: usize = self.static_data.inst[*pc].operand.0.into();
                *pc = self.static_data.funcs_pos[idx].func_addr;
                return Ok(());
            }
            Opcode::LoadGlobalVar => {
                let addr = self.static_data.inst[*pc].operand.0;
                let bytes_num = self.static_data.inst[*pc].operand.1;
                let var_begin_addr = self.dynadata.dydata.get_var_addr(addr.into());
                unsafe {
                    self.dynadata
                        .dydata
                        .write_to_stack(var_begin_addr as *mut Byte, bytes_num.into())
                };
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
                let bytes = self.static_data.inst[*pc].operand.0;
                self.dynadata.dydata.pop_n_bytes_data(bytes.into());
            }
            Opcode::JumpIfTrue => {
                let condit = impl_opcode!(bool, self, 1);
                if condit {
                    *pc = self.static_data.inst[*pc].operand.0.into();
                    return Ok(());
                }
            }
            Opcode::StoreLocal => {
                let byte_num = self.static_data.inst[*pc].operand.1;
                let var_addr = self.static_data.inst[*pc].operand.0;
                let stack_ptr = self.dynadata.dydata.pop_n_bytes_data(byte_num.into());
                unsafe {
                    self.dynadata.frames_stack.last_mut().unwrap().write_to(
                        var_addr.into(),
                        stack_ptr,
                        byte_num.into(),
                    )
                };
            }
            Opcode::StoreGlobal => {
                let var_addr = self.static_data.inst[*pc].operand.0;
                let bytes_num = self.static_data.inst[*pc].operand.1;
                let stack_data_ptr = self.dynadata.dydata.pop_n_bytes_data(*bytes_num as usize);
                // println!(
                //     "opcode num:{}.notice:{} {} {}",
                //     *pc,
                //     var_addr as usize,
                //     bytes_num as usize,
                //     unsafe { *(stack_data_ptr as *mut TrcIntInternal) }
                // );
                // println!("previous:{}", unsafe {
                //     *(self.dynadata.dydata.get_var_addr(8) as *mut Byte)
                // });
                unsafe {
                    self.dynadata.dydata.write_to_var(
                        var_addr.into(),
                        stack_data_ptr,
                        bytes_num.into(),
                    )
                };
                // println!("after:{}", unsafe {
                //     *(self.dynadata.dydata.get_var_addr(8) as *mut Byte)
                // });
            }
        };
        *pc += 1;
        Ok(())
    }

    /// 运行代码
    pub fn run(&mut self) -> RuntimeResult<()> {
        self.reset()?;
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
        std::io::stdout().flush().unwrap();
        Ok(())
    }

    /// 导入一个dll模块
    fn import_module(&mut self, i: String) -> Result<libloading::Library, RuntimeError> {
        let lib = unsafe {
            match libloading::Library::new(i.clone()) {
                Ok(lib) => lib,
                Err(_) => {
                    return self.report_err(module_not_found(&i));
                }
            }
        };
        let (_module, storage) = crate::base::dll::load_module_storage(&lib);
        for i in storage.func_table() {
            self.dynadata.imported_func.push(*i)
        }
        Ok(lib)
    }
}

#[cfg(test)]
mod tests {}
