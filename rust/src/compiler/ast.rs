use super::scope::SymScope;
use super::{token::Token, TokenLex};
use crate::base::codegen::{Inst, Opcode};
use crate::base::{codegen::StaticData, error::*};
use gettextrs::gettext;
use std::cell::RefCell;
use std::rc::Rc;

pub struct AstBuilder<'a> {
    token_lexer: TokenLex<'a>,
    staticdata: StaticData,
    self_scope: Rc<RefCell<SymScope>>,
}

impl<'a> AstBuilder<'a> {
    pub fn new(token_lexer: TokenLex<'a>) -> Self {
        let mut root_scope = Rc::new(RefCell::new(SymScope::new(None)));
        AstBuilder {
            token_lexer,
            staticdata: StaticData::new(),
            self_scope: root_scope,
        }
    }

    fn while_lex(&mut self) -> RunResult<()> {
        Ok(())
    }

    fn for_lex(&mut self) -> RunResult<()> {
        Ok(())
    }

    fn generate_block(&mut self, t: Token) -> RunResult<()> {
        Ok(())
    }

    fn expr(&mut self) -> RunResult<()> {
        Ok(())
    }

    fn statement(&mut self, mut t: Token) -> RunResult<()> {
        match t.tp {
            super::token::TokenType::ID => {
                let name = t.data.unwrap();
                let tt = self.token_lexer.next_token()?;
                match tt.tp {
                    super::token::TokenType::Assign => {
                        let var = self.self_scope.as_ref().borrow().get_sym_idx(name);
                        if let None = var {
                            return Err(RuntimeError::new(
                                Box::new(self.token_lexer.compiler_data.content.clone()),
                                ErrorInfo::new(
                                    gettext!(
                                        SYMBOL_NOT_FOUND,
                                        self.token_lexer.compiler_data.const_pool.id_name[name]
                                    ),
                                    gettext(SYMBOL_ERROR),
                                ),
                            ));
                        }
                        self.expr()?;
                        let var = var.unwrap();
                        self.staticdata
                            .inst
                            .push(Inst::new(Opcode::StoreLocal, var))
                    }
                    super::token::TokenType::Store => {
                        if self.self_scope.as_ref().borrow().has_sym(name) {
                            return Err(RuntimeError::new(
                                Box::new(self.token_lexer.compiler_data.content.clone()),
                                ErrorInfo::new(
                                    gettext!(
                                        SYMBOL_REDEFINED,
                                        self.token_lexer.compiler_data.const_pool.id_name[name]
                                    ),
                                    gettext(SYMBOL_ERROR),
                                ),
                            ));
                        }
                        self.expr()?;
                        let var_idx = self.self_scope.as_ref().borrow_mut().insert_sym(name);
                        self.staticdata.update_sym_table_sz(
                            self.self_scope.as_ref().borrow().get_scope_last_idx(),
                        );
                        self.staticdata
                            .inst
                            .push(Inst::new(Opcode::StoreLocal, var_idx))
                    }
                    _ => {
                        return Err(RuntimeError::new(
                            Box::new(self.token_lexer.compiler_data.content.clone()),
                            ErrorInfo::new(
                                gettext!(UNEXPECTED_TOKEN, tt.tp.to_string()),
                                gettextrs::gettext(SYNTAX_ERROR),
                            ),
                        ))
                    }
                }
            }
            _ => {
                return Err(RuntimeError::new(
                    Box::new(self.token_lexer.compiler_data.content.clone()),
                    ErrorInfo::new(
                        gettext!(UNEXPECTED_TOKEN, t.tp.to_string()),
                        gettextrs::gettext(SYNTAX_ERROR),
                    ),
                ))
            }
        }
        Ok(())
    }

    pub fn generate_code(&mut self) -> RunResult<()> {
        loop {
            let token = self.token_lexer.next_token()?;
            if token.tp == super::token::TokenType::EndOfFile {
                break;
            }
            self.statement(token)?;
        }
        return Ok(());
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::compiler::Compiler;

    macro_rules! gen_test_env {
        ($test_code:expr, $env_name:ident) => {{
            use crate::compiler::InputSource;
            use crate::compiler::Option;
            let mut compiler = Compiler::new_string_compiler(
                Option::new(false, InputSource::StringInternal),
                $test_code,
            );
            let mut token_lexer = TokenLex::new(&mut compiler);
            let mut $env_name = AstBuilder::new(token_lexer);
        }};
    }

    #[test]
    fn test_assign() {
        gen_test_env!(r#"a:=10"#, t);
    }

    #[test]
    fn builtin_function_call() {}
}
