//! the read execute print loop for trc

use colored::*;
use rust_i18n::t;
use std::io::{self, Write};

use crate::{
    base::{codegen::StaticData, error::RunResult},
    compiler::{self, token::TokenType},
    tvm::Vm,
};

pub fn tshell() -> RunResult<()> {
    println!("{}\n\n", t!("tshell.welcome").bold());
    let mut compiler = compiler::Compiler::new_string_compiler(
        compiler::Option::new(false, compiler::InputSource::StringInternal),
        "",
    );
    let mut ast = compiler.lex()?;
    let mut vm = unsafe { Vm::new(&*(ast.prepare_get_static() as *const StaticData)) };
    loop {
        print!("tshell>");
        io::stdout().flush().unwrap();

        let mut line = String::new();
        let mut cnt = 0;
        // 此处要引入compiler的词法分析器来解析大括号和小括号
        let mut braces_lexer = compiler::Compiler::new_string_compiler(
            compiler::Option::new(false, compiler::InputSource::StringInternal),
            "",
        );
        let mut check_lexer = braces_lexer.get_token_lex();
        let mut flag = true;
        loop {
            for _ in 0..cnt {
                print!("....");
            }
            io::stdout().flush().unwrap();

            let mut tmp = String::new();
            io::stdin().read_line(&mut tmp).unwrap();
            line += &tmp;
            check_lexer.modify_input(Box::new(compiler::StringSource::new(tmp)));
            loop {
                let i = match check_lexer.next_token() {
                    Ok(i) => i,
                    Err(e) => {
                        eprintln!("{}", e);
                        flag = false;
                        break;
                    }
                };
                if i.tp == TokenType::EndOfFile {
                    break;
                }
                if i.tp == compiler::token::TokenType::LeftBigBrace {
                    cnt += 1;
                } else if i.tp == compiler::token::TokenType::RightBigBrace {
                    cnt -= 1;
                }
            }
            if cnt == 0 {
                break;
            }
        }
        if !flag {
            continue;
        }
        let source = Box::new(compiler::StringSource::new(line));
        ast.token_lexer.modify_input(source);
        ast.clear_inst();
        ast.token_lexer.clear_error();
        ast.generate_code().unwrap_or_else(|e| {
            eprintln!("{}", e);
        });
        vm.set_static_data(unsafe { &*(ast.prepare_get_static() as *const StaticData) });
        match vm.run() {
            Ok(_) => {}
            Err(e) => {
                eprintln!("{}", e);
            }
        }
    }
}

mod tests {}
