//! ast的控制者，总管一个编译过程中的数据

use libcore::StaticData;

use crate::compiler::{ast::ModuleUnit, linker::link, optimizer::optimize_module};
use std::collections::{hash_map::IterMut, HashMap};

/// ast manager
pub struct ModuleManager {
    // 储存模块名字和对应的模块路径的关系
    modules: HashMap<String, String>,
    // 有的模块是本次已经编译过的，在缓存中可以直接找到
    cache: HashMap<String, ModuleUnit>,
    global_custom_function_id: usize,
    global_extern_function_id: usize,
}

pub struct LinkerIter<'a> {
    cache_iter: IterMut<'a, String, ModuleUnit>,
}

impl<'a> Iterator for LinkerIter<'a> {
    type Item = &'a mut StaticData;
    fn next(&mut self) -> Option<Self::Item> {
        match self.cache_iter.next() {
            None => None,
            Some((_, v)) => Some(v.prepare_get_static()),
        }
    }
}

impl<'a> LinkerIter<'a> {
    pub fn new(cache_iter: IterMut<'a, String, ModuleUnit>) -> Self {
        Self { cache_iter }
    }
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

    pub fn add_module(&mut self, path: impl Into<String>, module: ModuleUnit) {
        self.cache.insert(path.into(), module);
    }

    pub fn add_specific_module(&mut self, name: String, path: String) {
        self.modules.insert(name, path);
    }

    pub fn get_module<T>(&mut self, name: &str) -> Option<&ModuleUnit> {
        // 查找已经存在的别名
        match self.modules.get(name) {
            None => None::<T>,
            Some(v) => return self.cache.get(v),
        };
        // 没找到，从路径中找
        match self.cache.get(name) {
            None => None,
            Some(v) => return Some(v),
        }
        // 还是没找到，从磁盘中加载中间文件
        // 磁盘中不存在但是对应模块存在，重新编译
        // 对应模块不存在，返回None
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
        // 有的模块在cache中，有的需要从ctrc中加载
        let s = self.cache.iter_mut();
        let iter = LinkerIter::new(s);
        link(iter)
    }
}

impl Default for ModuleManager {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {}
