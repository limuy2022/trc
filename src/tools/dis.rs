use crate::base::codegen::Opcode::*;
use crate::base::error::RunResult;

pub fn dis(opt: crate::compiler::Option) -> RunResult<()> {
    let mut compiler = crate::compiler::Compiler::new(opt);
    let mut ast = compiler.lex()?;
    let static_data = ast.prepare_get_static();
    for i in static_data.inst.iter().enumerate() {
        print!("{} {}", i.0, i.1);
        match i.1.opcode {
            LoadInt => print!("({})", static_data.constpool.intpool[i.1.operand]),
            LoadString => print!("({})", static_data.constpool.stringpool[i.1.operand]),
            LoadFloat => print!("({})", static_data.constpool.floatpool[i.1.operand]),
            _ => {}
        }
        println!();
    }
    Ok(())
}
