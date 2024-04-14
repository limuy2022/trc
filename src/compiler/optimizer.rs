use crate::compiler::AstBuilder;

/// 对单个模块的优化器
pub struct LocalOptimizer {}

impl LocalOptimizer {
    pub fn new() -> LocalOptimizer {
        Self {}
    }

    /// 优化模块
    pub fn optimize_module(&mut self, module: AstBuilder) -> AstBuilder {
        todo!()
    }
}

impl Default for LocalOptimizer {
    fn default() -> Self {
        Self::new()
    }
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
