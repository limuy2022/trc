use core::{cmp::max, fmt};
use std::{fmt::Display, usize};

#[derive(Clone)]
pub struct FuncStorage {
    pub func_addr: usize,
    pub var_table_sz: usize,
}

impl FuncStorage {
    pub fn new(name: usize, var_table_sz: usize) -> Self {
        Self {
            func_addr: name,
            var_table_sz,
        }
    }
}

#[derive(Debug, PartialEq, Clone, Copy)]
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
    LoadLocalVarObj,
    LoadLocalVarBool,
    LoadLocalVarInt,
    LoadLocalVarFloat,
    LoadLocalVarStr,
    LoadLocalVarChar,
    LoadGlobalVarObj,
    LoadGlobalVarBool,
    LoadGlobalVarInt,
    LoadGlobalVarFloat,
    LoadGlobalVarStr,
    LoadGlobalVarChar,
    // Move a value into the stack
    MoveInt,
    MoveFloat,
    MoveChar,
    MoveBool,
    MoveStr,
    // Store a local var
    StoreLocalObj,
    StoreLocalInt,
    StoreLocalFloat,
    StoreLocalChar,
    StoreLocalBool,
    StoreLocalStr,
    StoreGlobalObj,
    StoreGlobalInt,
    StoreGlobalFloat,
    StoreGlobalChar,
    StoreGlobalBool,
    StoreGlobalStr,
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
    PopDataInt,
    PopDataFloat,
    PopDataStr,
    PopDataChar,
    PopDataBool,
    ImportNativeModule,
}

impl Display for Opcode {
    #[cfg(not(tarpaulin_include))]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{:?}", self)
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
    pub operand: usize,
}

impl Inst {
    pub fn new(opcode: Opcode, operand: usize) -> Self {
        Self { opcode, operand }
    }
}

impl Display for Inst {
    #[cfg(not(tarpaulin_include))]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} {}", self.opcode, self.operand)
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
    pub funcs: Vec<FuncStorage>,
    // 符号表需要的长度
    pub sym_table_sz: usize,
    pub line_table: Vec<usize>,
    pub type_list: Vec<Vec<VmStackType>>,
    pub function_split: Option<usize>,
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
}
