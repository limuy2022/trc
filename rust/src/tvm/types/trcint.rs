use super::super::Opcode;

pub struct TrcInt {
    pub value: i64,
}

impl TrcInt {
    pub fn new(value: i64) -> TrcInt {
        TrcInt { value }
    }

    pub fn check_opcode_support(opcode: Opcode) -> bool {
        match opcode {
            _ => false,
        }
    }
}
