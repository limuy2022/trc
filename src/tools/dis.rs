use crate::base::codegen::Opcode::*;
use crate::base::error::RunResult;

pub fn dis(opt: crate::compiler::Option) -> RunResult<()> {
    let mut compiler = crate::compiler::Compiler::new(opt);
    let mut ast = compiler.lex()?;
    let static_data = ast.prepare_get_static();
    for i in &static_data.inst {
        print!("{}", i);
        match i.opcode {
            LoadInt => print!("({})", static_data.constpool.intpool[i.operand]),
            LoadString => print!("({})", static_data.constpool.stringpool[i.operand]),
            LoadFloat => print!("({})", static_data.constpool.floatpool[i.operand]),
            _ => {}
        }
        println!();
    }
    Ok(())
}
