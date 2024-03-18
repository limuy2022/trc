use super::super::types::*;
use crate::base::stdlib::*;
use crate::tvm::{TrcIntInternal, TrcStrInternal};
use crate::{base::error::*, tvm::DynaData};
use derive::{def_module, trc_function};
use rust_i18n::t;
use std::io::{self, Write};

#[trc_function(var_params = true)]
pub fn print(fmt_string: str) -> void {
    let mut iter = va_list.iter();
    let mut output_iter = unsafe { (*fmt_string).chars() };
    while let Some(i) = output_iter.next() {
        if i == '{' {
            unsafe {
                io::stdout()
                    .write_all((**iter.next().unwrap()).to_string().as_bytes())
                    .unwrap();
            }
            if let Some(j) = output_iter.next() {
                if j != '}' {
                    return Err(ErrorInfo::new(t!(UNCLOSED_FORMAT), t!(FORMAT_STR_ERROR)));
                }
            }
        } else {
            io::stdout().write_all(&[i as u8]).unwrap();
        }
    }
}

#[trc_function(var_params = true)]
pub fn println(fmt_string: str) -> void {
    let mut iter = va_list.iter();
    let mut output_iter = unsafe { (*fmt_string).chars() };
    while let Some(i) = output_iter.next() {
        if i == '{' {
            unsafe {
                io::stdout()
                    .write_all((**iter.next().unwrap()).to_string().as_bytes())
                    .unwrap();
            }
            if let Some(j) = output_iter.next() {
                if j != '}' {
                    return Err(ErrorInfo::new(t!(UNCLOSED_FORMAT), t!(FORMAT_STR_ERROR)));
                }
            }
        } else {
            io::stdout().write_all(&[i as u8]).unwrap();
        }
    }
    io::stdout().write_all(b"\n").unwrap();
}

def_module!(module_name = prelude, functions = [print => print, println => println], classes = [
    TrcInt => int,
    TrcStr => str,
    TrcBool => bool,
    TrcFloat => float,
    TrcChar => char
]);
