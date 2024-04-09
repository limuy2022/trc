use libcore::*;

pub fn dis(opt: crate::compiler::CompileOption, rustcode: bool) -> RuntimeResult<()> {
    let mut compiler = crate::compiler::Compiler::new(opt);
    let mut ast = compiler.lex()?;
    let static_data = ast.prepare_get_static();
    for i in static_data.inst.iter().enumerate() {
        if rustcode {
            println!("Inst::new(Opcode::{}, {}),", i.1.opcode, i.1.operand);
        } else {
            print!("{} {}", i.0, i.1);
            match i.1.opcode {
                Opcode::LoadInt => print!("({})", static_data.constpool.intpool[i.1.operand]),
                Opcode::LoadString => print!("({})", static_data.constpool.stringpool[i.1.operand]),
                Opcode::LoadFloat => print!("({})", static_data.constpool.floatpool[i.1.operand]),
                _ => {}
            }
            println!();
        }
    }
    Ok(())
}
