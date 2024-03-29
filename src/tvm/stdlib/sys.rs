use derive::{def_module, trc_function};

#[trc_function]
pub fn exit(val: int) -> void {
    std::process::exit(val as i32);
}

def_module!(module_name = sys, functions = [exit => exit]);
