//! ast的控制者，总管一个编译过程中的数据

use libcore::StaticData;

use crate::compiler::{ast::AstBuilder, optimizer::optimize_module};
use std::collections::HashMap;

/// ast manager
pub struct ModuleManager {
    // 储存模块和对应的中间文件的关系
    modules: HashMap<String, String>,
    // 有的模块是本次已经编译过的，在缓存中可以直接找到
    cache: HashMap<String, AstBuilder>,
    global_custom_function_id: usize,
    global_extern_function_id: usize,
}

impl ModuleManager {
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            cache: HashMap::new(),
            global_custom_function_id: 0,
            global_extern_function_id: 0,
        }
    }

    pub fn add_module(&mut self, path: impl Into<String>, module: AstBuilder) {
        self.cache.insert(path.into(), module);
    }

    pub fn add_specific_module(&mut self, name: String, path: String) {
        self.modules.insert(name, path);
    }

    pub fn get_module(&mut self, name: &str) -> Option<&AstBuilder> {
        todo!()
    }

    pub fn alloc_custom_function_id(&mut self) -> usize {
        let ret = self.global_custom_function_id;
        self.global_custom_function_id += 1;
        ret
    }

    pub fn alloc_extern_function_id(&mut self) -> usize {
        let ret = self.global_extern_function_id;
        self.global_extern_function_id += 1;
        ret
    }

    /// 执行优化
    pub fn optimize(&mut self) {
        for i in &mut self.cache {
            optimize_module(i.1);
        }
    }

    /// 将所有模块都链接到一起
    pub fn link(&mut self) -> StaticData {
        todo!()
    }
}

impl Default for ModuleManager {
    fn default() -> Self {
        Self::new()
    }
}
