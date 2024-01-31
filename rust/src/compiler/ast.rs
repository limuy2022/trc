use super::scope::SymScope;
use super::token::TokenType;
use super::{token::Token, TokenLex};
use crate::base::codegen::{Inst, Opcode, NO_ARG};
use crate::base::{codegen::StaticData, error::*};
use gettextrs::gettext;
use std::cell::RefCell;
use std::rc::Rc;

pub struct AstBuilder<'a> {
    token_lexer: TokenLex<'a>,
    staticdata: StaticData,
    self_scope: Rc<RefCell<SymScope>>,
}

#[derive(PartialEq, Debug)]
enum LexState {
    Success,
    Failure,
}

type AstError = RunResult<LexState>;

macro_rules! Trythrow {
    ($istry:expr, $($error_info:expr),*) => {
        {
        if $istry {
            Ok(LexState::Failure)
        } else {
            Err(RuntimeError::new(
                $($error_info),*
            ))
        }
        }
    };
}

/// there are a log of similar operators to be generated
macro_rules! ExprGen {
    ($funcname:ident, $tmpfuncname:ident, $next_item_func:ident, $($accepted_token:path => $add_opcode:path),*) => {
        fn $tmpfuncname(&mut self, istry: bool) -> AstError {
            let next_sym = self.token_lexer.next_token()?;
            match next_sym.tp {
                $($accepted_token => {
                    self.$next_item_func(istry)?;
                    self.staticdata.inst.push(Inst::new($add_opcode, NO_ARG));
                })*
                _ => {}
            }
            Ok(LexState::Success)
        }
        fn $funcname(&mut self, istry: bool) -> AstError {
            self.$next_item_func(istry)?;
            self.$tmpfuncname(istry)?;
            Ok(LexState::Success)
        }
    };
}

macro_rules! SignleExprGen {
    ($funcname:ident, $next_item_func:ident, $($accepted_token:path => $add_opcode:path),*) => {
        fn $funcname(&mut self, istry: bool) -> AstError {
            let tmp = self.token_lexer.next_token()?;
            match tmp.tp {
                $($accepted_token => {
                    self.$next_item_func(istry)?;
                    self.staticdata.inst.push(Inst::new($add_opcode, NO_ARG));
                })*
                _ => {
                    self.token_lexer.next_back(tmp);
                    self.$next_item_func(istry)?;
                }
            }
            Ok(LexState::Success)
        }
    };
}

impl<'a> AstBuilder<'a> {
    pub fn new(token_lexer: TokenLex<'a>) -> Self {
        let root_scope = Rc::new(RefCell::new(SymScope::new(None)));
        AstBuilder {
            token_lexer,
            staticdata: StaticData::new(),
            self_scope: root_scope,
        }
    }

    ExprGen!(expr11, expr11_, factor, TokenType::Power => Opcode::Power);
    SignleExprGen!(expr10, expr11, TokenType::Add => Opcode::Empty,
    TokenType::Sub => Opcode::SelfNegative,
    TokenType::BitNot => Opcode::BitNot
    );
    ExprGen!(expr9, expr9_, expr10, TokenType::Mul => Opcode::Mul,
    TokenType::Div => Opcode::Div,
    TokenType::Mod => Opcode::Mod,
    TokenType::ExactDiv => Opcode::ExtraDiv);
    ExprGen!(expr8, expr8_, expr9, TokenType::Add => Opcode::Add,
        TokenType::Sub => Opcode::Sub);
    ExprGen!(expr7, expr7_, expr8, TokenType::BitLeftShift => Opcode::BitLeftShift,
        TokenType::BitRightShift => Opcode::BitRightShift);
    ExprGen!(expr6, expr6_, expr7, TokenType::BitAnd => Opcode::BitAnd);
    ExprGen!(expr5, expr5_, expr6, TokenType::Xor => Opcode::Xor);
    ExprGen!(expr4, expr4_, expr5, TokenType::BitOr => Opcode::BitOr);
    ExprGen!(expr3, expr3_, expr4, TokenType::Equal => Opcode::Eq,
        TokenType::NotEqual => Opcode::Ne,
        TokenType::Less => Opcode::Lt,
        TokenType::LessEqual => Opcode::Le,
        TokenType::Greater => Opcode::Gt,
        TokenType::GreaterEqual => Opcode::Ge
    );
    SignleExprGen!(expr2, expr3, TokenType::Not => Opcode::Not);
    ExprGen!(expr1, expr1_, expr2, TokenType::And => Opcode::And);
    ExprGen!(expr, expr_, expr1, TokenType::Or => Opcode::Or);

    fn while_lex(&mut self, istry: bool) -> AstError {
        Ok(LexState::Success)
    }

    fn for_lex(&mut self, istry: bool) -> AstError {
        Ok(LexState::Success)
    }

    fn generate_block(&mut self, istry: bool) -> AstError {
        Ok(LexState::Success)
    }

    fn check_next_token(&mut self, tp: TokenType) -> AstError {
        let next_sym = self.token_lexer.next_token()?;
        if next_sym.tp != tp {
            return Err(RuntimeError::new(
                Box::new(self.token_lexer.compiler_data.content.clone()),
                ErrorInfo::new(
                    gettext!(UNEXPECTED_TOKEN, next_sym.tp.to_string()),
                    gettextrs::gettext(SYNTAX_ERROR),
                ),
            ));
        }
        Ok(LexState::Success)
    }

    fn val(&mut self, istry: bool) -> AstError {
        Ok(LexState::Success)
    }

    fn item(&mut self, istry: bool) -> AstError {
        match self.val(istry) {
            Err(e) => {
                return Err(e);
            }
            Ok(v) => {
                if v == LexState::Success {
                    return Ok(LexState::Success);
                }
            }
        }
        let t = self.token_lexer.next_token()?;
        match t.tp {
            TokenType::IntValue => {
                self.staticdata
                    .inst
                    .push(Inst::new(Opcode::LoadInt, t.data.unwrap()));
            }
            TokenType::FloatValue => {
                self.staticdata
                    .inst
                    .push(Inst::new(Opcode::LoadFloat, t.data.unwrap()));
            }
            TokenType::StringValue => {
                self.staticdata
                    .inst
                    .push(Inst::new(Opcode::LoadString, t.data.unwrap()));
            }
            _ => {
                panic!("unreachable!")
            }
        }

        Ok(LexState::Success)
    }

    fn factor(&mut self, istry: bool) -> AstError {
        let next_token = self.token_lexer.next_token()?;
        if next_token.tp == TokenType::LeftSmallBrace {
            self.expr(istry)?;
            self.check_next_token(TokenType::RightSmallBrace)?;
        } else {
            self.token_lexer.next_back(next_token);
            self.item(istry)?;
        }
        Ok(LexState::Success)
    }

    fn def_func(&mut self, istry: bool) -> RunResult<LexState> {
        Ok(LexState::Success)
    }

    fn def_class(&mut self, istry: bool) -> RunResult<LexState> {
        Ok(LexState::Success)
    }

    fn func_call(&mut self, istry: bool) -> RunResult<LexState> {
        Ok(LexState::Success)
    }

    fn statement(&mut self, t: Token) -> RunResult<()> {
        match t.tp {
            super::token::TokenType::Func => {
                self.def_func(false)?;
            }
            super::token::TokenType::Class => {
                self.def_class(false)?;
            }
            super::token::TokenType::While => {
                self.while_lex(false)?;
            }
            super::token::TokenType::For => {
                self.for_lex(false)?;
            }
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
                        self.expr(false)?;
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
                        self.expr(false)?;
                        let var_idx = self.self_scope.as_ref().borrow_mut().insert_sym(name);
                        self.staticdata.update_sym_table_sz(
                            self.self_scope.as_ref().borrow().get_scope_last_idx(),
                        );
                        self.staticdata
                            .inst
                            .push(Inst::new(Opcode::StoreLocal, var_idx))
                    }
                    super::token::TokenType::LeftSmallBrace => {
                        self.func_call(false)?;
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
            _ => {}
        }
        self.token_lexer.next_back(t.clone());
        match self.expr(true) {
            Ok(_) => {}
            Err(_) => {
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
            if token.tp == super::token::TokenType::EndOfLine {
                continue;
            }
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
    use crate::compiler::{Compiler, INT_VAL_POOL_ONE};

    macro_rules! gen_test_env {
        ($test_code:expr, $env_name:ident) => {
            use crate::compiler::InputSource;
            use crate::compiler::Option;
            let mut compiler = Compiler::new_string_compiler(
                Option::new(false, InputSource::StringInternal),
                $test_code,
            );
            let mut token_lexer = TokenLex::new(&mut compiler);
            let mut $env_name = AstBuilder::new(token_lexer);
        };
    }

    #[test]
    fn test_assign() {
        gen_test_env!(r#"a:=10"#, t);
    }

    #[test]
    fn test_builtin_function_call() {}

    #[test]
    fn test_expr() {
        gen_test_env!(r#"1+9-10*7**6"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::Add, NO_ARG),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(Opcode::LoadInt, 5),
                Inst::new(Opcode::Power, NO_ARG),
                Inst::new(Opcode::Mul, NO_ARG),
                Inst::new(Opcode::Sub, NO_ARG),
            ]
        );
    }
}
