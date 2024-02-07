use crate::base::stdlib::{new_class_id, Stdlib, ANY_TYPE, STD_CLASS_TABLE};
use derive::def_module;

pub mod algo;
pub mod ds;
pub mod prelude;

def_module!(module_name = std, submodules = [prelude, ds, algo]);

pub fn import_stdlib() -> Stdlib {
    let _newid = new_class_id();
    unsafe {
        STD_CLASS_TABLE.push(ANY_TYPE.clone());
    }

    init()
}
