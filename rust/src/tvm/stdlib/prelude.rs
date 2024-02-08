use std::io::{self, Write};

use super::super::types::*;
use crate::base::stdlib::*;
use crate::{base::error::*, tvm::DynaData};
use derive::{def_module, trc_function};
use gettextrs::gettext;

#[trc_function(var_params = true)]
pub fn print(fmt_string: str) -> void {
    let mut iter = va_list.iter();
    let mut output_iter = fmt_string.chars();
    while let Some(i) = output_iter.next() {
        if i == '{' {
            io::stdout()
                .write_all(iter.next().unwrap().to_string().as_bytes())
                .unwrap();
            if let Some(j) = output_iter.next() {
                if j != '}' {
                    return Err(ErrorInfo::new(
                        gettext(UNCLOSED_FORMAT),
                        gettext(FORMAT_STR_ERROR),
                    ));
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
    let mut output_iter = fmt_string.chars();
    while let Some(i) = output_iter.next() {
        if i == '{' {
            io::stdout()
                .write_all(iter.next().unwrap().to_string().as_bytes())
                .unwrap();
            if let Some(j) = output_iter.next() {
                if j != '}' {
                    return Err(ErrorInfo::new(
                        gettext(UNCLOSED_FORMAT),
                        gettext(FORMAT_STR_ERROR),
                    ));
                }
            }
        } else {
            io::stdout().write_all(&[i as u8]).unwrap();
        }
    }
    io::stdout().write_all(b"\n").unwrap();
}

def_module!(module_name = prelude, functions = [print => print, println => print], classes = [
    TrcInt => int,
    TrcStr => str,
    TrcBool => bool,
    TrcFloat => float,
    TrcChar => char
]);
