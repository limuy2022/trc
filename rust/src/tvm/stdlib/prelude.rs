use super::super::types::*;
use crate::base::stdlib::*;
use crate::{base::error::*, tvm::DynaData};
use derive::{def_module, trc_function};

#[trc_function]
pub fn print(obj: any) -> void {
    print!("{}", obj);
}

#[trc_function]
pub fn println(obj: any) -> void {
    println!("{}", obj);
}

def_module!(module_name = prelude, functions = [print => print, println => print], classes = [
    TrcInt => int,
    TrcStr => str,
    TrcBool => bool,
    TrcFloat => float,
    TrcChar => char
]);
