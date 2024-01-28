use super::{token::Token, Compiler, TokenLex};
use crate::base::{codegen::StaticData, error::*};
use clap::error;
use gettextrs::gettext;

pub struct AstBuilder<'a> {
    token_lexer: TokenLex<'a>,
    staticdata: StaticData,
}

impl<'a> AstBuilder<'a> {
    pub fn new(token_lexer: TokenLex<'a>) -> Self {
        AstBuilder {
            token_lexer,
            staticdata: StaticData::new(),
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

    fn statement(&mut self, mut t: Token) -> RunResult<()> {
        match t.tp {
            super::token::TokenType::ID => {
                t = self.token_lexer.next_token()?;
                match t.tp {
                    super::token::TokenType::Assign => {}
                    super::token::TokenType::Store => {}
                    _ => {
                        return Err(RuntimeError::new(
                            Box::new(self.token_lexer.compiler_data.content.clone()),
                            ErrorInfo::new(
                                gettextrs::gettext(SYNTAX_ERROR),
                                gettext!(UNEXPECTED_TOKEN, t.tp.to_string()),
                            ),
                        ))
                    }
                }
            }
            _ => {
                return Err(RuntimeError::new(
                    Box::new(self.token_lexer.compiler_data.content.clone()),
                    ErrorInfo::new(
                        gettextrs::gettext(SYNTAX_ERROR),
                        gettext!(UNEXPECTED_TOKEN, t.tp.to_string()),
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
    #[test]
    fn test_assign() {}

    #[test]
    fn builtin_function_call() {}
}
