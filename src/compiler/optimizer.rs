use crate::compiler::AstBuilder;

/// 优化模块
pub fn optimize_module(module: &mut AstBuilder) {
    todo!()
}

pub struct GlobalOptimizer {}

impl GlobalOptimizer {
    pub fn new() -> GlobalOptimizer {
        Self {}
    }
}

impl Default for GlobalOptimizer {
    fn default() -> Self {
        Self::new()
    }
}
