use derive::{def_module, trc_function};
use libcore::*;

#[trc_function]
#[allow(unreachable_code)]
pub fn exit(val: int) -> void {
    std::process::exit(val as i32);
}

def_module!(module_name = sys, functions = [exit => exit]);
