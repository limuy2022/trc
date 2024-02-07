use super::token::TokenType;
use super::TokenLex;
use super::{scope::*, InputSource};
use crate::base::codegen::{Inst, Opcode, NO_ARG};
use crate::base::stdlib::{RustFunction, STDLIB_ROOT};
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

macro_rules! try_err {
    ($istry: expr, $($argvs:expr),*) => {
        {if $istry {
            Err(LightFakeError::new().into())
        } else {
            Err(RuntimeError::new($($argvs),*))
        }
    }
    };
}

macro_rules! tmp_expe_function_gen {
    ($tmpfuncname:ident, $next_item_func:ident, $($accepted_token:path => $add_opcode:path),*) => {
        fn $tmpfuncname(&mut self, istry: bool, extend: usize) -> AstError<TypeAllowNull> {
            let next_sym = self.token_lexer.next_token()?;
            match next_sym.tp {
                $($accepted_token => {
                    let tya = self.$next_item_func(istry)?.unwrap();
                    // 读取IOType检查
                    let func_obj = self.self_scope.as_ref().borrow().get_class(extend).unwrap();
                    let io_check = func_obj.get_override_func($accepted_token);
                    match io_check {
                        None => return try_err!(istry,
                            Box::new(self.token_lexer.compiler_data.content.clone()),
                            ErrorInfo::new(
                                gettext!(OPERATOR_IS_NOT_SUPPORT, $accepted_token, func_obj.get_name()),
                                gettext(OPERATOR_ERROR),
                            )
                        ),
                        Some(v) => {
                            if let Ok(_) = v.check_argvs(vec![tya]) {}
                            else {
                                let func_obja = self.self_scope.as_ref().borrow().get_class(tya).unwrap();
                                return try_err!(istry,
                                    Box::new(self.token_lexer.compiler_data.content.clone()),
                                    ErrorInfo::new(gettext!(OPERATOR_IS_NOT_SUPPORT, $accepted_token, func_obj.get_name()), gettextrs::gettext(OPERATOR_ERROR)))
                            }
                        }
                    }
                    let io_check = io_check.unwrap();
                    self.add_bycode($add_opcode, NO_ARG);
                    let stage_ty = io_check.return_type.unwrap();
                    let tyb = self.$tmpfuncname(istry, stage_ty)?;
                    match tyb {
                        TypeAllowNull::No => {
                            return Ok(TypeAllowNull::Yes(stage_ty));
                        }
                        TypeAllowNull::Yes(_) => {
                            return Ok(tyb);
                        }
                    }
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
macro_rules! expr_gen {
    ($funcname:ident, $tmpfuncname:ident, $next_item_func:ident, $($accepted_token:path => $add_opcode:path),*) => {
        tmp_expe_function_gen!($tmpfuncname, $next_item_func, $($accepted_token => $add_opcode),*);
        fn $funcname(&mut self, istry: bool) -> AstError<TypeAllowNull> {
            let t1 = self.$next_item_func(istry)?;
            if let TypeAllowNull::No = t1 {
                return Ok(t1);
            }
            let t2 = self.$tmpfuncname(istry, t1.unwrap())?;
            if let TypeAllowNull::No = t2 {
                return Ok(t1);
            }
            Ok(t2)
        }
    };
}

impl<'a> AstBuilder<'a> {
    pub fn new(token_lexer: TokenLex<'a>) -> Self {
        let prelude = STDLIB_ROOT.sub_modules.get("prelude").unwrap().clone();
        for i in prelude.functions {
            token_lexer.compiler_data.const_pool.add_id(i.0.clone());
        }
        for i in prelude.classes {
            token_lexer.compiler_data.const_pool.add_id(i.0.clone());
        }
        let root_scope = Rc::new(RefCell::new(SymScope::new(None)));
        // 为root scope添加prelude
        let optimize = token_lexer.compiler_data.option.optimize;
        let ret = AstBuilder {
            token_lexer,
            staticdata: StaticData::new(!optimize),
            self_scope: root_scope,
        };
        ret.self_scope
            .as_ref()
            .borrow_mut()
            .import_prelude(&ret.token_lexer.compiler_data.const_pool);
        ret
    }

    expr_gen!(expr9, expr9_, factor, TokenType::Power => Opcode::Power);
    expr_gen!(expr8, expr8_, expr9, TokenType::Mul => Opcode::Mul,
    TokenType::Div => Opcode::Div,
    TokenType::Mod => Opcode::Mod,
    TokenType::ExactDiv => Opcode::ExtraDiv);
    expr_gen!(expr7, expr7_, expr8, TokenType::Add => Opcode::Add,
        TokenType::Sub => Opcode::Sub);
    expr_gen!(expr6, expr6_, expr7, TokenType::BitLeftShift => Opcode::BitLeftShift,
        TokenType::BitRightShift => Opcode::BitRightShift);
    expr_gen!(expr5, expr5_, expr6, TokenType::BitAnd => Opcode::BitAnd);
    expr_gen!(expr4, expr4_, expr5, TokenType::Xor => Opcode::Xor);
    expr_gen!(expr3, expr3_, expr4, TokenType::BitOr => Opcode::BitOr);
    expr_gen!(expr2, expr2_, expr3, TokenType::Equal => Opcode::Eq,
        TokenType::NotEqual => Opcode::Ne,
        TokenType::Less => Opcode::Lt,
        TokenType::LessEqual => Opcode::Le,
        TokenType::Greater => Opcode::Gt,
        TokenType::GreaterEqual => Opcode::Ge
    );
    expr_gen!(expr1, expr1_, expr2, TokenType::And => Opcode::And);
    expr_gen!(expr, expr_, expr1, TokenType::Or => Opcode::Or);

    pub fn return_static_data(mut self) -> StaticData {
        self.staticdata.constpool = self.token_lexer.compiler_data.const_pool.store_val_to_vm();
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
                    gettext!(UNEXPECTED_TOKEN, next_sym.tp),
                    gettextrs::gettext(SYNTAX_ERROR),
                ),
            ));
        }
        Ok(())
    }

    fn opt_args(&mut self) -> AstError<Vec<usize>> {
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
        return if t.tp == TokenType::ID {
            let token_data = t.data.unwrap();
            let idx = self.self_scope.as_ref().borrow().get_sym_idx(token_data);
            if let None = idx {
                return try_err!(
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
                let tmp = self.self_scope.as_ref().borrow();
                let func_obj = tmp.get_function(idx).unwrap();
                match func_obj.get_io().check_argvs(argv_list) {
                    Err(e) => {
                        return try_err!(
                            istry,
                            Box::new(self.token_lexer.compiler_data.content.clone()),
                            e
                        )
                    }
                    Ok(_) => {}
                }
                drop(tmp);
                if let Some(obj) = func_obj.downcast_ref::<RustFunction>() {
                    self.add_bycode(Opcode::CallNative, obj.buildin_id);
                }
                Ok(func_obj.get_io().return_type.clone())
            } else {
                self.token_lexer.next_back(nxt);
                let varidx = self.self_scope.as_ref().borrow_mut().insert_sym(idx);
                self.add_bycode(Opcode::LoadLocal, varidx);
                let tt = self.self_scope.as_ref().borrow().get_type(varidx);
                Ok(TypeAllowNull::Yes(tt))
            }
        } else {
            self.token_lexer.next_back(t.clone());
            try_err!(
                istry,
                Box::new(self.token_lexer.compiler_data.content.clone()),
                ErrorInfo::new(
                    gettext!(UNEXPECTED_TOKEN, t.tp),
                    gettextrs::gettext(SYNTAX_ERROR),
                )
            )
        };
    }

    fn item(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        if let Ok(v) = self.val(true) {
            return Ok(v);
        }
        let t = self.token_lexer.next_token()?;
        return match t.tp {
            TokenType::IntValue => {
                self.add_bycode(Opcode::LoadInt, t.data.unwrap());
                Ok(TypeAllowNull::Yes(
                    self.self_scope.as_ref().borrow().get_type(
                        *self
                            .token_lexer
                            .compiler_data
                            .const_pool
                            .name_pool
                            .get("int")
                            .unwrap(),
                    ),
                ))
            }
            TokenType::FloatValue => {
                self.add_bycode(Opcode::LoadFloat, t.data.unwrap());
                Ok(TypeAllowNull::Yes(
                    self.self_scope.as_ref().borrow().get_type(
                        *self
                            .token_lexer
                            .compiler_data
                            .const_pool
                            .name_pool
                            .get("float")
                            .unwrap(),
                    ),
                ))
            }
            TokenType::StringValue => {
                self.add_bycode(Opcode::LoadString, t.data.unwrap());
                Ok(TypeAllowNull::Yes(
                    self.self_scope.as_ref().borrow().get_type(
                        *self
                            .token_lexer
                            .compiler_data
                            .const_pool
                            .name_pool
                            .get("str")
                            .unwrap(),
                    ),
                ))
            }
            _ => {
                self.token_lexer.next_back(t.clone());
                try_err!(
                    istry,
                    Box::new(self.token_lexer.compiler_data.content.clone()),
                    ErrorInfo::new(
                        gettext!(UNEXPECTED_TOKEN, t.tp),
                        gettextrs::gettext(SYNTAX_ERROR),
                    )
                )
            }
        };
    }

    fn factor(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        let next_token = self.token_lexer.next_token()?;
        return match next_token.tp {
            TokenType::Sub => {
                let ret = self.factor(istry)?;
                self.add_bycode(Opcode::SelfNegative, NO_ARG);
                Ok(ret)
            }
            TokenType::BitNot => {
                let ret = self.factor(istry)?;
                self.add_bycode(Opcode::BitNot, NO_ARG);
                Ok(ret)
            }
            TokenType::Not => {
                let ret = self.factor(istry)?;
                self.add_bycode(Opcode::Not, NO_ARG);
                Ok(ret)
            }
            TokenType::Add => Ok(self.factor(istry)?),
            TokenType::LeftSmallBrace => {
                let ret = self.expr(istry)?;
                self.check_next_token(TokenType::RightSmallBrace)?;
                Ok(ret)
            }
            _ => {
                self.token_lexer.next_back(next_token);
                Ok(self.item(istry)?)
            }
        };
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
        let path = self.token_lexer.next_token()?;
        if path.tp != TokenType::StringValue {
            return try_err!(
                istry,
                Box::new(self.token_lexer.compiler_data.content.clone()),
                ErrorInfo::new(
                    gettext!(UNEXPECTED_TOKEN, path.tp),
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
            TokenType::Func => {
                self.def_func(false)?;
            }
            TokenType::Class => {
                self.def_class(false)?;
            }
            TokenType::While => {
                self.while_lex(false)?;
            }
            TokenType::For => {
                self.for_lex(false)?;
            }
            TokenType::ID => {
                let name = t.data.unwrap();
                let tt = self.token_lexer.next_token()?;
                match tt.tp {
                    TokenType::Assign => {
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
                        self.add_bycode(Opcode::StoreLocal, var)
                    }
                    TokenType::Store => {
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
                        self.add_bycode(Opcode::StoreLocal, var_idx)
                    }
                    _ => {
                        self.token_lexer.next_back(tt);
                    }
                }
            }
            _ => {}
        }
        self.token_lexer.next_back(t.clone());
        self.expr(false)?;
        Ok(())
    }

    pub fn generate_code(&mut self) -> RunResult<()> {
        loop {
            let token = self.token_lexer.next_token()?;
            if token.tp == TokenType::EndOfFile {
                break;
            }
            self.token_lexer.next_back(token);
            self.statement()?;
        }
        return Ok(());
    }

    fn add_bycode(&mut self, opty: Opcode, opnum: usize) {
        self.staticdata.inst.push(Inst::new(opty, opnum));
        if !self.token_lexer.compiler_data.option.optimize {
            // 不生成行号表了
            self.staticdata
                .line_table
                .push(self.token_lexer.compiler_data.content.get_line())
        }
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
                Inst::new(
                    Opcode::CallNative,
                    STDLIB_ROOT
                        .sub_modules
                        .get("prelude")
                        .unwrap()
                        .functions
                        .get("print")
                        .unwrap()
                        .buildin_id
                ),
            ]
        )
    }

    #[test]
    #[should_panic(expected = "OperatorError")]
    fn test_wrong_type() {
        gen_test_env!(r#"1.0+9"#, t);
        t.generate_code().unwrap();
    }

    #[test]
    #[should_panic(expected = "OperatorError")]
    fn test_wrong_type2() {
        gen_test_env!(r#"1+"90""#, t);
        t.generate_code().unwrap();
    }

    #[test]
    #[should_panic(expected = "OperatorError")]
    fn test_wrong_type3() {
        gen_test_env!(r#""90"+28"#, t);
        t.generate_code().unwrap();
    }
}
