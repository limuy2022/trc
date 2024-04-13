//! ast的控制者，总管一个编译过程中的数据

use crate::compiler::ast::AstBuilder;
use std::collections::HashMap;

/// ast manager
pub struct ModuleManager<'a> {
    // 储存模块和对应的中间文件的关系
    modules: HashMap<String, String>,
    // 有的模块是本次已经编译过的，在缓存中可以直接找到
    cache: HashMap<String, AstBuilder<'a>>,
    global_custom_function_id: usize,
    global_extern_function_id: usize,
}

impl<'a> ModuleManager<'a> {
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            cache: HashMap::new(),
            global_custom_function_id: 0,
            global_extern_function_id: 0,
        }
    }

    pub fn add_module(&mut self, path: String, module: AstBuilder<'a>) {
        self.cache.insert(path, module);
    }

    pub fn add_specific_module(&mut self, name: String, path: String) {
        self.modules.insert(name, path);
    }

    pub fn get_module(&mut self, name: &str) -> Option<&AstBuilder<'a>> {
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
}

impl<'a> Default for ModuleManager<'a> {
    fn default() -> Self {
        Self::new()
    }
}
