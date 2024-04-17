use core::{cmp::max, fmt};
use std::fmt::Display;

use num_enum::TryFromPrimitive;

#[derive(Clone)]
pub struct FuncStorage {
    pub func_addr: usize,
    pub var_table_sz: usize,
}

impl FuncStorage {
    pub fn new(func_addr: usize, var_table_sz: usize) -> Self {
        Self {
            func_addr,
            var_table_sz,
        }
    }
}

pub type OpcodeTy = u16;

#[derive(Debug, PartialEq, Clone, Copy, TryFromPrimitive)]
#[repr(u16)]
pub enum Opcode {
    Add,
    AddInt,
    AddFloat,
    AddStr,
    Sub,
    SubInt,
    SubFloat,
    Mul,
    MulInt,
    MulFloat,
    Div,
    DivInt,
    DivFloat,
    ExactDiv,
    ExactDivInt,
    ExtraDivFloat,
    Mod,
    ModInt,
    Power,
    PowerInt,
    Eq,
    EqInt,
    EqFloat,
    EqStr,
    EqChar,
    EqBool,
    Ne,
    NeInt,
    NeFloat,
    NeStr,
    NeChar,
    NeBool,
    Lt,
    LtInt,
    LtFloat,
    Le,
    LeInt,
    LeFloat,
    Gt,
    GtInt,
    GtFloat,
    Ge,
    GeInt,
    GeFloat,
    And,
    AndBool,
    Or,
    OrBool,
    Not,
    NotBool,
    Xor,
    XorInt,
    BitNot,
    BitNotInt,
    BitAnd,
    BitAndInt,
    BitOr,
    BitOrInt,
    BitLeftShift,
    BitLeftShiftInt,
    BitRightShift,
    BitRightShiftInt,
    // return from a function
    PopFrame,
    // Load a int from const pool
    LoadInt,
    // Load a float from const pool
    LoadFloat,
    // Load a string from const pool
    LoadString,
    // Load a bigint from const pool
    LoadBigInt,
    LoadChar,
    LoadBool,
    // Load a local var to the stack
    LoadLocalVar,
    LoadGlobalVar,
    // Move a value into the stack
    MoveInt,
    MoveFloat,
    MoveChar,
    MoveBool,
    MoveStr,
    // Store a local var
    StoreLocal,
    StoreGlobal,
    // Do Nothing
    Empty,
    // a = -a
    SelfNegative,
    SelfNegativeInt,
    SelfNegativeFloat,
    // call native func
    CallNative,
    CallCustom,
    JumpIfFalse,
    JumpIfTrue,
    Jump,
    Stop,
    EqWithoutPop,
    EqIntWithoutPop,
    EqFloatWithoutPop,
    EqStrWithoutPop,
    EqCharWithoutPop,
    EqBoolWithoutPop,
    PopData,
}

impl Display for Opcode {
    #[cfg(not(tarpaulin_include))]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl Opcode {
    /// 获取操作数个数
    pub fn get_opcode_arg_nums(&self) -> usize {
        use Opcode::*;
        match *self {
            Add | AddInt | AddFloat | AddStr | Sub | SubInt | SubFloat | Mul | MulInt
            | MulFloat | Div | DivInt | DivFloat | ExactDiv | ExactDivInt | ExtraDivFloat | Mod
            | ModInt | Power | PowerInt | Eq | EqInt | EqFloat | EqStr | EqChar | EqBool | Ne
            | NeInt | NeFloat | NeStr | NeChar | NeBool | Lt | LtInt | LtFloat | Le | LeInt
            | LeFloat | Gt | GtInt | GtFloat | Ge | GeInt | GeFloat | And | AndBool | Or
            | OrBool | Not | NotBool | Xor | XorInt | BitNot | BitNotInt | BitAnd | BitAndInt
            | BitOr | BitOrInt | BitLeftShift | BitLeftShiftInt | BitRightShift
            | BitRightShiftInt | PopFrame | MoveInt | MoveFloat | MoveChar | MoveBool | MoveStr
            | SelfNegative | SelfNegativeInt | SelfNegativeFloat | Empty | Stop | EqWithoutPop
            | EqIntWithoutPop | EqFloatWithoutPop | EqStrWithoutPop | EqCharWithoutPop
            | EqBoolWithoutPop => 0,
            PopData | LoadInt | LoadFloat | LoadString | LoadBigInt | LoadChar | LoadBool
            | CallNative | CallCustom | JumpIfFalse | JumpIfTrue | Jump => 1,
            LoadLocalVar | LoadGlobalVar | StoreLocal | StoreGlobal => 2,
        }
    }
}

#[derive(Default, Clone)]
pub struct ConstPool {
    pub intpool: Vec<i64>,
    pub stringpool: Vec<String>,
    pub floatpool: Vec<f64>,
}

impl ConstPool {
    pub fn new() -> Self {
        Self {
            ..Default::default()
        }
    }
}

pub const NO_ARG: usize = 0;
pub const ARG_WRONG: usize = usize::MAX;

#[derive(Debug, PartialEq, Clone)]
pub struct Inst {
    pub opcode: Opcode,
    pub operand: (usize, usize),
}

impl Inst {
    pub fn new_single(opcode: Opcode, operand: usize) -> Self {
        Self {
            opcode,
            operand: (operand, ARG_WRONG),
        }
    }

    pub fn new_double(opcode: Opcode, operand1: usize, operand2: usize) -> Self {
        Self {
            opcode,
            operand: (operand1, operand2),
        }
    }
}

impl Display for Inst {
    #[cfg(not(tarpaulin_include))]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} {} {}", self.opcode, self.operand.0, self.operand.1)
    }
}

#[derive(Clone, Copy)]
pub enum VmStackType {
    Int,
    Float,
    Str,
    Char,
    Bool,
    Object,
}

pub type InstSet = Vec<Inst>;

#[derive(Default, Clone)]
pub struct StaticData {
    pub constpool: ConstPool,
    pub inst: InstSet,
    // 储存函数的位置
    pub funcs_pos: Vec<FuncStorage>,
    // 符号表需要的长度
    pub sym_table_sz: usize,
    pub line_table: Vec<usize>,
    pub type_list: Vec<Vec<VmStackType>>,
    pub function_split: Option<usize>,
    pub dll_module_should_loaded: Vec<String>,
}

impl StaticData {
    pub fn new() -> StaticData {
        Self {
            ..Default::default()
        }
    }

    #[inline]
    pub fn update_var_table_mem_sz(&mut self, newsz: usize) {
        self.sym_table_sz = max(self.sym_table_sz, newsz);
    }

    #[inline]
    pub fn get_last_opcode_id(&self) -> usize {
        self.inst.len() - 1
    }

    #[inline]
    pub fn get_next_opcode_id(&self) -> usize {
        self.inst.len()
    }

    #[inline]
    pub fn add_dll_module(&mut self, module_name: String) {
        self.dll_module_should_loaded.push(module_name);
    }
}