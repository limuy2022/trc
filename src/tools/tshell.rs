//! the read execute print loop for trc

use crate::{
    compiler::{self, token::TokenType},
    tvm::Vm,
};
use colored::*;
use libcore::{codegen::StaticData, *};
use rust_i18n::t;
use rustyline::{config::Configurer, history::FileHistory, Editor};
use std::io::{self, Write};

pub fn tshell() -> RuntimeResult<()> {
    println!("{}\n", t!("tshell.welcome").bold());
    let config = rustyline::config::Config::builder()
        .check_cursor_position(true)
        .build();
    let mut rl: Editor<(), FileHistory> = rustyline::Editor::with_config(config).unwrap();
    rl.set_max_history_size(1000).unwrap();
    let mut compiler = compiler::Compiler::new_string_compiler(
        compiler::CompileOption::new(false, compiler::InputSource::StringInternal),
        "",
    );
    let mut ast = compiler.lex()?;
    let mut vm = unsafe { Vm::new(&*(ast.prepare_get_static() as *const StaticData)) };
    let mut should_exit = false;
    let mut function_list = vec![];
    let mut function_defined_num = 0;
    'stop_repl: loop {
        let mut line = String::new();
        let mut cnt = 0;
        // 此处要引入compiler的词法分析器来解析大括号和小括号
        let mut braces_lexer = compiler::Compiler::new_string_compiler(
            compiler::CompileOption::new(false, compiler::InputSource::StringInternal),
            "",
        );
        let mut check_lexer = braces_lexer.get_token_lex();
        let mut flag = true;
        let mut first_read = true;
        loop {
            if !first_read {
                line.push('\n');
            }
            first_read = false;
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
                if i.tp == TokenType::LeftBigBrace {
                    cnt += 1;
                } else if i.tp == TokenType::RightBigBrace {
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
        if let Err(e) = ast.generate_code() {
            eprintln!("{}", e);
            continue;
        }
        // 将之前的函数添加进去
        if !function_list.is_empty() {
            ast.add_bycode(Opcode::Stop, NO_ARG);
            for i in 0..function_defined_num {
                ast.staticdata.funcs[i].func_addr += ast.staticdata.get_next_opcode_id();
            }
            ast.staticdata.inst.extend(function_list.clone());
        }
        vm.set_static_data(unsafe { &*(ast.prepare_get_static() as *const StaticData) });
        match vm.run() {
            Ok(_) => {}
            Err(e) => {
                eprintln!("{}", e);
            }
        }
        // 切分之前定义的函数
        if let Some(func_split) = ast.staticdata.function_split {
            function_list = ast.staticdata.inst[func_split + 1..].to_vec();
            function_defined_num = ast.staticdata.funcs.len();
            for i in &mut ast.staticdata.funcs {
                i.func_addr -= func_split + 1;
            }
        } else {
            let mut basis = usize::MAX;
            for i in &ast.staticdata.funcs {
                if i.func_addr < basis {
                    basis = i.func_addr;
                }
            }
            for i in &mut ast.staticdata.funcs {
                i.func_addr -= basis;
            }
        }
        io::stdout().flush().unwrap();
        should_exit = false;
    }
    Ok(())
}

mod tests {}
