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
    println!("deps modules:");
    for i in &static_data.dll_module_should_loaded {
        println!("{}", i);
    }
    println!();
    for i in static_data.inst.iter().enumerate() {
        if rustcode {
            if i.1.operand.1 == ARG_WRONG {
                println!(
                    "Inst::new_single(Opcode::{}, {}),",
                    i.1.opcode, i.1.operand.0
                );
            } else {
                println!(
                    "Inst::new_double(Opcode::{}, {}, {}),",
                    i.1.opcode, i.1.operand.0, i.1.operand.1
                );
            }
        } else {
            print!("{} {}", i.0, i.1);
            match i.1.opcode {
                Opcode::LoadInt => print!("({})", static_data.constpool.intpool[i.1.operand.0]),
                Opcode::LoadString => {
                    print!("({})", static_data.constpool.stringpool[i.1.operand.0])
                }
                Opcode::LoadFloat => {
                    print!("({})", static_data.constpool.floatpool[i.1.operand.0])
                }
                _ => {}
            }
            println!();
        }
    }

    Ok(())
}
