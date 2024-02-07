use std::borrow::Borrow;

use crate::base::stdlib::{
    new_class_id, ClassInterface, RustClass, Stdlib, ANY_TYPE, STD_CLASS_TABLE,
};
use derive::def_module;
use std::collections::HashMap;

pub mod algo;
pub mod ds;
pub mod prelude;

def_module!(module_name = std, submodules = [prelude, ds, algo]);

pub fn import_stdlib() -> Stdlib {
    STD_CLASS_TABLE.with(|std| {
        let newid = new_class_id();
        std.borrow_mut().push(ANY_TYPE.clone())
    });
    let ret = init();
    ret
}
