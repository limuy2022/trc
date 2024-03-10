//! the read execute print loop for trc

use std::io::{self, Write};

use colored::*;
use rust_i18n::t;
use rustyline::{config::Configurer, history::FileHistory, Editor};

use crate::{
    base::{codegen::StaticData, error::RunResult},
    compiler::{self, token::TokenType},
    tvm::Vm,
};

pub fn tshell() -> RunResult<()> {
    println!("{}\n", t!("tshell.welcome").bold());
    let config = rustyline::config::Config::builder()
        .check_cursor_position(true)
        .build();
    // let mut rl = rustyline::DefaultEditor::new().unwrap();
    let mut rl: Editor<(), FileHistory> = rustyline::Editor::with_config(config).unwrap();
    rl.set_max_history_size(1000).unwrap();
    let mut compiler = compiler::Compiler::new_string_compiler(
        compiler::Option::new(false, compiler::InputSource::StringInternal),
        "",
    );
    let mut ast = compiler.lex()?;
    let mut vm = unsafe { Vm::new(&*(ast.prepare_get_static() as *const StaticData)) };
    let mut should_exit = false;
    'stop_repl: loop {
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
            let tip_msg = "....".repeat(cnt) + ">>>>";
            let tmp = match rl.readline(&tip_msg) {
                Ok(val) => val,
                Err(rustyline::error::ReadlineError::Interrupted) => {
                    if should_exit {
                        break 'stop_repl;
                    }
                    flag = false;
                    should_exit = true;
                    break;
                }
                Err(rustyline::error::ReadlineError::Eof) => {
                    break 'stop_repl;
                }
                Err(err) => {
                    panic!("{}", err);
                }
            };
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
        rl.add_history_entry(line.clone()).unwrap();
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
        io::stdout().flush().unwrap();
        should_exit = false;
    }
    Ok(())
}

mod tests {}
