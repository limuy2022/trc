use crate::compiler::ModuleUnit;

/// 优化模块
pub fn optimize_module(_module: &mut ModuleUnit) {
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
