use crate::base::stdlib::{get_any_type, new_class_id, Stdlib, STD_CLASS_TABLE};
use derive::def_module;

pub mod algo;
pub mod ds;
pub mod math;
pub mod prelude;
pub mod sys;

def_module!(module_name = std, submodules = [prelude, ds, algo, math, sys]);

/// 导入标准库
/// # Warning
/// 这个函数在整个程序中只被调用一次，不能调用第二次
pub fn import_stdlib() -> Stdlib {
    // this is for any type
    let _newid = new_class_id();
    // 该函数不会并行调用，所以这里是safe的
    unsafe {
        STD_CLASS_TABLE.push(get_any_type().clone());
    }
    init()
}
