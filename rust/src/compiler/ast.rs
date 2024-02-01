use super::token::TokenType;
use super::TokenLex;
use super::{scope::*, InputSource};
use crate::base::codegen::{Inst, Opcode, NO_ARG};
use crate::base::stdlib::{RustFunction, STDLIB_LIST};
use crate::base::{codegen::StaticData, error::*};
use gettextrs::gettext;
use std::cell::RefCell;
use std::rc::Rc;

pub struct AstBuilder<'a> {
    token_lexer: TokenLex<'a>,
    staticdata: StaticData,
    self_scope: Rc<RefCell<SymScope>>,
}

type AstError<T> = RunResult<T>;

macro_rules! TryErr {
    ($istry: expr, $($argvs:expr),*) => {
        {if $istry {
            Err(LightFakeError::new().into())
        } else {
            Err(RuntimeError::new($($argvs),*))
        }
    }
    };
}

macro_rules! TmpExpeFunctionGen {
    ($tmpfuncname:ident, $next_item_func:ident, $($accepted_token:path => $add_opcode:path),*) => {
        fn $tmpfuncname(&mut self, istry: bool) -> AstError<TypeAllowNull> {
            let next_sym = self.token_lexer.next_token()?;
            match next_sym.tp {
                $($accepted_token => {
                    let ty_now = self.$next_item_func(istry)?;
                    self.staticdata.inst.push(Inst::new($add_opcode, NO_ARG));
                    let ty_after = self.$tmpfuncname(istry)?;
                    if let TypeAllowNull::No = ty_after {
                        return Ok(ty_now);
                    }
                    if(ty_now == ty_after) {
                        return Ok(ty_now);
                    }
                    return TryErr!(istry,
                            Box::new(self.token_lexer.compiler_data.content.clone()),
                            ErrorInfo::new(gettext!(TYPE_NOT_THE_SAME, ty_now,
                                    ty_after), gettextrs::gettext(TYPE_ERROR)))
                })*
                _ => {
                    self.token_lexer.next_back(next_sym);
                    return Ok(TypeAllowNull::No);
                }
            }
        }
    };
}

/// there are a log of similar operators to be generated
macro_rules! ExprGen {
    ($funcname:ident, $tmpfuncname:ident, $next_item_func:ident, $($accepted_token:path => $add_opcode:path),*) => {
        TmpExpeFunctionGen!($tmpfuncname, $next_item_func, $($accepted_token => $add_opcode),*);
        fn $funcname(&mut self, istry: bool) -> AstError<TypeAllowNull> {
            let t1 = self.$next_item_func(istry)?;
            let t2 = self.$tmpfuncname(istry)?;
            if let TypeAllowNull::No = t2 {
                return Ok(t1);
            }
            if t1 != t2 {
                return TryErr!(istry,
                        Box::new(self.token_lexer.compiler_data.content.clone()),
                        ErrorInfo::new(gettext!(TYPE_NOT_THE_SAME, t1,
                                t2), gettextrs::gettext(TYPE_ERROR)))
            }
            Ok(t1)
        }
    };
}

impl<'a> AstBuilder<'a> {
    pub fn new(token_lexer: TokenLex<'a>) -> Self {
        let root_scope = Rc::new(RefCell::new(SymScope::new(None)));
        // 为root scope添加prelude
        let mut ret = AstBuilder {
            token_lexer,
            staticdata: StaticData::new(),
            self_scope: root_scope,
        };
        for i in &STDLIB_LIST
            .sons
            .get("prelude")
            .unwrap()
            .lock()
            .unwrap()
            .functions
        {
            ret.token_lexer
                .compiler_data
                .const_pool
                .add_id(i.name.clone());
        }
        ret.self_scope
            .as_ref()
            .borrow_mut()
            .import_prelude(&ret.token_lexer.compiler_data.const_pool);
        ret
    }

    ExprGen!(expr9, expr9_, factor, TokenType::Power => Opcode::Power);
    ExprGen!(expr8, expr8_, expr9, TokenType::Mul => Opcode::Mul,
    TokenType::Div => Opcode::Div,
    TokenType::Mod => Opcode::Mod,
    TokenType::ExactDiv => Opcode::ExtraDiv);
    ExprGen!(expr7, expr7_, expr8, TokenType::Add => Opcode::Add,
        TokenType::Sub => Opcode::Sub);
    ExprGen!(expr6, expr6_, expr7, TokenType::BitLeftShift => Opcode::BitLeftShift,
        TokenType::BitRightShift => Opcode::BitRightShift);
    ExprGen!(expr5, expr5_, expr6, TokenType::BitAnd => Opcode::BitAnd);
    ExprGen!(expr4, expr4_, expr5, TokenType::Xor => Opcode::Xor);
    ExprGen!(expr3, expr3_, expr4, TokenType::BitOr => Opcode::BitOr);
    ExprGen!(expr2, expr2_, expr3, TokenType::Equal => Opcode::Eq,
        TokenType::NotEqual => Opcode::Ne,
        TokenType::Less => Opcode::Lt,
        TokenType::LessEqual => Opcode::Le,
        TokenType::Greater => Opcode::Gt,
        TokenType::GreaterEqual => Opcode::Ge
    );
    ExprGen!(expr1, expr1_, expr2, TokenType::And => Opcode::And);
    ExprGen!(expr, expr_, expr1, TokenType::Or => Opcode::Or);

    pub fn return_static_data(self) -> StaticData {
        self.token_lexer.compiler_data.const_pool.store_val_to_vm();
        self.staticdata
    }

    fn while_lex(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        todo!()
    }

    fn for_lex(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        todo!()
    }

    fn generate_block(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        todo!()
    }

    fn check_next_token(&mut self, tp: TokenType) -> AstError<()> {
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
        Ok(())
    }

    fn opt_args(&mut self) -> AstError<Vec<Type>> {
        let mut ret = vec![];
        loop {
            let t = self.expr(true);
            match t {
                Err(_) => {
                    return Ok(ret);
                }
                Ok(ty) => match ty {
                    TypeAllowNull::No => {
                        return Ok(ret);
                    }
                    TypeAllowNull::Yes(t) => ret.push(t),
                },
            }
        }
    }

    fn val(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        let t = self.token_lexer.next_token()?;
        if t.tp == TokenType::ID {
            let token_data = t.data.unwrap();
            let idx = self.self_scope.as_ref().borrow().get_sym_idx(token_data);
            if let None = idx {
                return TryErr!(
                    istry,
                    Box::new(self.token_lexer.compiler_data.content.clone()),
                    ErrorInfo::new(
                        gettext!(
                            SYMBOL_NOT_FOUND,
                            self.token_lexer.compiler_data.const_pool.id_name[token_data]
                        ),
                        gettextrs::gettext(SYMBOL_ERROR),
                    )
                );
            }
            let idx = idx.unwrap();
            let nxt = self.token_lexer.next_token()?;
            if nxt.tp == TokenType::LeftSmallBrace {
                let argv_list = self.opt_args()?;
                // match )
                self.check_next_token(TokenType::RightSmallBrace)?;
                let tmp = self.self_scope.borrow();
                let func_obj = tmp.get_function(idx).unwrap();
                match func_obj.check_argvs(argv_list) {
                    Err(e) => {
                        return TryErr!(
                            istry,
                            Box::new(self.token_lexer.compiler_data.content.clone()),
                            e
                        )
                    }
                    Ok(_) => {}
                }
                if let Some(obj) = func_obj.downcast_ref::<RustFunction>() {
                    self.staticdata
                        .inst
                        .push(Inst::new(Opcode::CallNative, obj.buildin_id));
                }
                return Ok((*func_obj.get_return_type()).clone());
            } else {
                self.token_lexer.next_back(nxt);
                let varidx = self.self_scope.as_ref().borrow_mut().insert_sym(idx);
                self.staticdata
                    .inst
                    .push(Inst::new(Opcode::LoadLocal, varidx));
                let tt = self.self_scope.as_ref().borrow().get_type(varidx);
                return Ok(TypeAllowNull::Yes(Type::Common(tt)));
            }
        } else {
            self.token_lexer.next_back(t.clone());
            return TryErr!(
                istry,
                Box::new(self.token_lexer.compiler_data.content.clone()),
                ErrorInfo::new(
                    gettext!(UNEXPECTED_TOKEN, t.tp.to_string()),
                    gettextrs::gettext(SYNTAX_ERROR),
                )
            );
        }
    }

    fn item(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        if let Ok(v) = self.val(true) {
            return Ok(v);
        }
        let t = self.token_lexer.next_token()?;
        match t.tp {
            TokenType::IntValue => {
                self.staticdata
                    .inst
                    .push(Inst::new(Opcode::LoadInt, t.data.unwrap()));
                return Ok(TypeAllowNull::Yes(INT_TYPE.clone()));
            }
            TokenType::FloatValue => {
                self.staticdata
                    .inst
                    .push(Inst::new(Opcode::LoadFloat, t.data.unwrap()));
                return Ok(TypeAllowNull::Yes(FLOAT_TYPE.clone()));
            }
            TokenType::StringValue => {
                self.staticdata
                    .inst
                    .push(Inst::new(Opcode::LoadString, t.data.unwrap()));
                return Ok(TypeAllowNull::Yes(STR_TYPE.clone()));
            }
            _ => {
                self.token_lexer.next_back(t.clone());
                return TryErr!(
                    istry,
                    Box::new(self.token_lexer.compiler_data.content.clone()),
                    ErrorInfo::new(
                        gettext!(UNEXPECTED_TOKEN, t.tp.to_string()),
                        gettextrs::gettext(SYNTAX_ERROR),
                    )
                );
            }
        }
    }

    fn factor(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        let next_token = self.token_lexer.next_token()?;
        match next_token.tp {
            TokenType::Sub => {
                let ret = self.factor(istry)?;
                self.staticdata
                    .inst
                    .push(Inst::new(Opcode::SelfNegative, NO_ARG));
                return Ok(ret);
            }
            TokenType::BitNot => {
                let ret = self.factor(istry)?;
                self.staticdata.inst.push(Inst::new(Opcode::BitNot, NO_ARG));
                return Ok(ret);
            }
            TokenType::Not => {
                let ret = self.factor(istry)?;
                self.staticdata.inst.push(Inst::new(Opcode::Not, NO_ARG));
                return Ok(ret);
            }
            TokenType::Add => {
                return Ok(self.factor(istry)?);
            }
            TokenType::LeftSmallBrace => {
                let ret = self.expr(istry)?;
                self.check_next_token(TokenType::RightSmallBrace)?;
                return Ok(ret);
            }
            _ => {
                self.token_lexer.next_back(next_token);
                return Ok(self.item(istry)?);
            }
        }
    }

    fn def_func(&mut self, istry: bool) -> AstError<()> {
        Ok(())
    }

    fn def_class(&mut self, istry: bool) -> AstError<()> {
        Ok(())
    }

    fn func_call(&mut self, istry: bool) -> AstError<()> {
        Ok(())
    }

    fn import_module(&mut self, istry: bool) -> AstError<()> {
        let mut path = self.token_lexer.next_token()?;
        if path.tp != TokenType::StringValue {
            return TryErr!(
                istry,
                Box::new(self.token_lexer.compiler_data.content.clone()),
                ErrorInfo::new(
                    gettext!(UNEXPECTED_TOKEN, path.tp.to_string()),
                    gettextrs::gettext(SYNTAX_ERROR),
                )
            );
        }
        let path = std::path::PathBuf::from(
            self.token_lexer.compiler_data.const_pool.id_str[path.data.unwrap()]
                .clone()
                .replace('.', "/"),
        );
        // the standard library first
        let strpath = path.to_str().unwrap();
        if strpath.get(0..3) == Some("std") {
        } else {
            if let InputSource::File(now_module_path) =
                self.token_lexer.compiler_data.option.inputsource.clone()
            {
                let mut now_module_path = std::path::PathBuf::from(now_module_path);
                now_module_path.pop();
                now_module_path = now_module_path.join(path);
                if now_module_path.exists() {}
            }
        }
        Ok(())
    }

    fn statement(&mut self) -> RunResult<()> {
        let t = self.token_lexer.next_token()?;
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
                    _ => {
                        self.token_lexer.next_back(tt);
                    }
                }
            }
            _ => {}
        }
        self.token_lexer.next_back(t.clone());
        if let Ok(_) = self.expr(true) {
            return Ok(());
        }
        return Err(RuntimeError::new(
            Box::new(self.token_lexer.compiler_data.content.clone()),
            ErrorInfo::new(
                gettext!(UNEXPECTED_TOKEN, t.tp.to_string()),
                gettextrs::gettext(SYNTAX_ERROR),
            ),
        ));
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
            self.token_lexer.next_back(token);
            self.statement()?;
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
    fn test_expr_easy1() {
        gen_test_env!(r#"(1)"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE)]
        );
    }

    #[test]
    fn test_expr_easy2() {
        gen_test_env!(r#"5+~6"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::BitNot, NO_ARG),
                Inst::new(Opcode::Add, NO_ARG)
            ]
        );
    }

    #[test]
    fn text_expr_easy3() {
        gen_test_env!(r#"9-8-8"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::Sub, NO_ARG),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::Sub, NO_ARG)
            ]
        )
    }

    #[test]
    fn test_expr_easy4() {
        gen_test_env!(r#"(8-9)*7"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::Sub, NO_ARG),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(Opcode::Mul, NO_ARG)
            ]
        )
    }

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

    #[test]
    fn test_expr_final() {
        gen_test_env!(r#"(1+-2)*3/4**(5**6)==1||7==(8&9)"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::SelfNegative, NO_ARG),
                Inst::new(Opcode::Add, NO_ARG),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::Mul, NO_ARG),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(Opcode::LoadInt, 5),
                Inst::new(Opcode::LoadInt, 6),
                Inst::new(Opcode::Power, NO_ARG),
                Inst::new(Opcode::Power, NO_ARG),
                Inst::new(Opcode::Div, NO_ARG),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::Eq, NO_ARG),
                Inst::new(Opcode::LoadInt, 7),
                Inst::new(Opcode::LoadInt, 8),
                Inst::new(Opcode::LoadInt, 9),
                Inst::new(Opcode::BitAnd, NO_ARG),
                Inst::new(Opcode::Eq, NO_ARG),
                Inst::new(Opcode::Or, NO_ARG),
            ]
        );
    }

    #[test]
    fn test_call_builtin_function() {
        gen_test_env!(r#"print("hello world!")"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::CallNative, 0),
            ]
        )
    }
}
