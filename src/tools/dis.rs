use libcore::*;

use crate::base::ctrc::load_from_reader_without_magic;

pub fn dis(opt: crate::compiler::CompileOption, rustcode: bool) -> anyhow::Result<()> {
    let file = match std::fs::File::open(opt.inputsource.get_path()) {
        Ok(file) => file,
        Err(e) => {
            println!("open file error: {}", e);
            return Ok(());
        }
    };
    let mut reader = std::io::BufReader::new(file);
    let static_data = if crate::base::ctrc::check_if_ctrc_file(&mut reader)? {
        load_from_reader_without_magic(&mut reader)?
    } else {
        let mut compiler = crate::compiler::Compiler::new(opt);
        compiler.lex()?
    };
    // let static_data = ast.prepare_get_static();
    println!("deps modules(The order is significant):");
    for i in &static_data.dll_module_should_loaded {
        println!("{}", i);
    }
    println!("\nInst:");
    for i in static_data.inst.iter().enumerate() {
        if rustcode {
            if i.1.operand.1 == ARG_WRONG {
                println!(
                    "Inst::new_single(Opcode::{}, {}),",
                    i.1.opcode, *i.1.operand.0
                );
            } else {
                println!(
                    "Inst::new_double(Opcode::{}, {}, {}),",
                    i.1.opcode, *i.1.operand.0, *i.1.operand.1
                );
            }
        } else {
            print!("{}:{}", i.0, i.1.opcode);
            if i.1.operand.1 == ARG_WRONG {
                print!(" {}", *i.1.operand.0);
            } else {
                print!(" {} {}", *i.1.operand.0, *i.1.operand.1);
            }
            match i.1.opcode {
                Opcode::LoadInt => {
                    print!("({})", unsafe { convert_to_int_constval(i.1.operand.0) })
                }
                Opcode::LoadString => {
                    let idx: usize = i.1.operand.0.into();
                    print!("({})", static_data.constpool.stringpool[idx])
                }
                Opcode::LoadFloat => {
                    let idx: usize = i.1.operand.0.into();
                    print!("({})", static_data.constpool.floatpool[idx])
                }
                _ => {}
            }
            println!();
        }
    }
    println!("\nFunction Info:");
    for i in &static_data.funcs_pos {
        println!(
            "Function address:{} | Function Var table size:{}",
            i.func_addr, i.var_table_sz
        );
    }
    Ok(())
}
