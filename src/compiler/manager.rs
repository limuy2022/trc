use std::collections::HashMap;

use crate::compiler::ast::AstBuilder;

/// ast的控制者，总管一个编译过程中的数据

/// ast manager
pub struct AstManager<'a> {
    // 储存模块和对应的中间文件的关系
    modules: HashMap<String, String>,
    // 有的模块是本次已经编译过的，在缓存中可以直接找到
    cache: HashMap<String, AstBuilder<'a>>,
}

impl<'a> AstManager<'a> {
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            cache: HashMap::new(),
        }
    }
}

impl<'a> Default for AstManager<'a> {
    fn default() -> Self {
        Self::new()
    }
}
