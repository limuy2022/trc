use crate::base::stdlib::Stdlib;
use derive::def_module;

pub mod prelude;

def_module!(module_name = std, submodules = [prelude]);
