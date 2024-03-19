pub fn dis(path: &str) {
    let dis = std::fs::read_to_string(path).unwrap();
    // let mut compiler = compiler::Compiler::new(opt);
    // let tmp = compiler.lex();
    // match tmp {
    //     Ok(data) => {}
    //     Err(e) => {
    //         if dev {
    //             Err::<(), _>(e).unwrap();
    //         } else {
    //             eprintln!("{}", e);
    //             exit(1)
    //         }
    //     }
    // }
}
