use crate::compiler::scope::TyIdxTy;

/// 过程间分析用的结构
#[derive(Default)]
pub struct LexProcess {
    stack_type: Vec<TyIdxTy>,
    pub is_global: bool,
}

impl LexProcess {
    pub fn new() -> Self {
        Self {
            ..Default::default()
        }
    }

    pub fn new_type(&mut self, ty: TyIdxTy) {
        self.stack_type.push(ty);
    }

    pub fn clear(&mut self) {
        self.stack_type.clear();
    }

    pub fn get_last_ty(&self) -> Option<TyIdxTy> {
        self.stack_type.last().copied()
    }

    /// pop two val at the top of stack
    pub fn cal_val(&mut self, ty: TyIdxTy) {
        assert!(self.stack_type.len() >= 2);
        self.stack_type.pop();
        self.stack_type.pop();
        self.new_type(ty)
    }

    pub fn pop_last_ty(&mut self) -> Option<TyIdxTy> {
        self.stack_type.pop()
    }
}
