use super::token::TokenType;
use super::{scope, TokenLex};
use super::{scope::*, InputSource};
use crate::base::{
    codegen::{Inst, Opcode, StaticData, VmStackType, NO_ARG},
    error::*,
    stdlib::{get_stdlib, RustFunction},
};
use rust_i18n::t;
use std::{cell::RefCell, rc::Rc};

/// 过程间分析用的结构
#[derive(Default)]
struct LexProcess {
    stack_type: Vec<VmStackType>,
}

impl LexProcess {
    pub fn new() -> Self {
        Self {
            ..Default::default()
        }
    }

    pub fn new_type(&mut self, ty: VmStackType) {
        self.stack_type.push(ty);
    }

    pub fn get_last_ty(&self) -> Option<VmStackType> {
        self.stack_type.last().copied()
    }

    /// pop two val at the top of stack
    pub fn cal_val(&mut self, ty: VmStackType) {
        assert!(self.stack_type.len() >= 2);
        self.stack_type.pop();
        self.stack_type.pop();
        self.new_type(ty)
    }
}

pub struct AstBuilder<'a> {
    token_lexer: TokenLex<'a>,
    staticdata: StaticData,
    self_scope: Rc<RefCell<SymScope>>,
    process_info: LexProcess,
}

type AstError<T> = RunResult<T>;

macro_rules! tmp_expe_function_gen {
    ($tmpfuncname:ident, $next_item_func:ident, $($accepted_token:path),*) => {
        fn $tmpfuncname(&mut self, istry: bool, extend: usize) -> AstError<TypeAllowNull> {
            let next_sym = self.token_lexer.next_token()?;
            match next_sym.tp {
                $($accepted_token => {
                    let tya = self.$next_item_func(istry)?.unwrap();
                    // 读取IOType检查
                    let func_obj = self.self_scope.as_ref().borrow().get_class(extend).unwrap();
                    let io_check = func_obj.get_override_func($accepted_token);
                    match io_check {
                        None => self.try_err(istry,
                            ErrorInfo::new(
                                t!(OPERATOR_IS_NOT_SUPPORT, "0"=$accepted_token, "1"=func_obj.get_name()),
                                t!(OPERATOR_ERROR),
                            )
                        )?,
                        Some(v) => {
                            if let Ok(_) = v.io.check_argvs(vec![tya]) {}
                            else {
                                self.try_err(istry,
                                    ErrorInfo::new(t!(OPERATOR_IS_NOT_SUPPORT, "0"=$accepted_token, "1"=func_obj.get_name()), t!(OPERATOR_ERROR)))?
                            }
                        }
                    }
                    let io_check = io_check.unwrap();
                    self.add_bycode(io_check.opcode.clone(), NO_ARG);
                    let stage_ty = io_check.io.return_type.unwrap();
                    let tyb = self.$tmpfuncname(istry, stage_ty)?;
                    self.process_info.cal_val(self.convert_to_vm_type(stage_ty));
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
    ($funcname:ident, $tmpfuncname:ident, $next_item_func:ident, $($accepted_token:path),*) => {
        tmp_expe_function_gen!($tmpfuncname, $next_item_func, $($accepted_token),*);
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
    #[inline]
    pub fn try_err<T>(&self, istry: bool, info: ErrorInfo) -> AstError<T> {
        if istry {
            Err(LightFakeError::new().into())
        } else {
            self.token_lexer.compiler_data.report_compiler_error(info)
        }
    }

    pub fn new(token_lexer: TokenLex<'a>) -> Self {
        let prelude = get_stdlib().sub_modules.get("prelude").unwrap();
        for i in &prelude.functions {
            token_lexer.compiler_data.const_pool.add_id(i.0.clone());
        }
        for i in &prelude.classes {
            token_lexer.compiler_data.const_pool.add_id(i.0.clone());
        }
        let root_scope = Rc::new(RefCell::new(SymScope::new(None)));
        // 为root scope添加prelude
        let optimize = token_lexer.compiler_data.option.optimize;
        let ret = AstBuilder {
            token_lexer,
            staticdata: StaticData::new(!optimize),
            self_scope: root_scope,
            process_info: LexProcess::new(),
        };
        ret.self_scope
            .as_ref()
            .borrow_mut()
            .import_prelude(&ret.token_lexer.compiler_data.const_pool);
        ret
    }

    expr_gen!(expr9, expr9_, factor, TokenType::Power);
    expr_gen!(
        expr8,
        expr8_,
        expr9,
        TokenType::Mul,
        TokenType::Div,
        TokenType::Mod,
        TokenType::ExactDiv
    );
    expr_gen!(expr7, expr7_, expr8, TokenType::Sub, TokenType::Add);
    expr_gen!(
        expr6,
        expr6_,
        expr7,
        TokenType::BitLeftShift,
        TokenType::BitRightShift
    );
    expr_gen!(expr5, expr5_, expr6, TokenType::BitAnd);
    expr_gen!(expr4, expr4_, expr5, TokenType::Xor);
    expr_gen!(expr3, expr3_, expr4, TokenType::BitOr);
    expr_gen!(
        expr2,
        expr2_,
        expr3,
        TokenType::Equal,
        TokenType::NotEqual,
        TokenType::Less,
        TokenType::LessEqual,
        TokenType::Greater,
        TokenType::GreaterEqual
    );
    expr_gen!(expr1, expr1_, expr2, TokenType::And);
    expr_gen!(expr, expr_, expr1, TokenType::Or);

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
            self.token_lexer
                .compiler_data
                .report_compiler_error(ErrorInfo::new(
                    t!(UNEXPECTED_TOKEN, "0" = next_sym.tp),
                    t!(SYNTAX_ERROR),
                ))?;
        }
        Ok(())
    }

    fn add_var_params_bycode(&mut self, var_params_num: usize) {
        let tmp = self
            .token_lexer
            .compiler_data
            .const_pool
            .add_int(var_params_num as i64);
        self.add_bycode(Opcode::LoadInt, tmp);
    }

    /// 解析出函数参数
    fn opt_args(&mut self, lex_func_obj: &scope::Func) -> AstError<Vec<usize>> {
        let mut ret = vec![];
        let mut var_params_num = 0;
        let io_tmp = lex_func_obj.get_io();
        loop {
            let t = self.expr(true)?;
            match t {
                TypeAllowNull::No => {
                    self.token_lexer
                        .compiler_data
                        .report_compiler_error(ErrorInfo::new(
                            t!(ARGUMENT_CANNOT_BE_VOID),
                            t!(ARGUMENT_ERROR),
                        ))?;
                }
                TypeAllowNull::Yes(t) => {
                    // 如果是可变参数是需要将其转入obj_stack的
                    if io_tmp.var_params && io_tmp.argvs_type.len() <= ret.len() {
                        // the values that have been stored is more than exact requirement of function
                        self.move_val_into_obj_stack();
                        var_params_num += 1;
                    }
                    ret.push(t)
                }
            }
            let nextt = self.token_lexer.next_token()?;
            match nextt.tp {
                TokenType::RightSmallBrace => {
                    if io_tmp.var_params {
                        self.add_var_params_bycode(var_params_num);
                    }
                    self.token_lexer.next_back(nextt);
                    return Ok(ret);
                }
                TokenType::Comma => {}
                _ => {
                    self.token_lexer.next_back(nextt);
                }
            }
        }
    }

    fn get_type_id(&self, ty_name: &str) -> usize {
        self.self_scope.as_ref().borrow().get_type(
            *self
                .token_lexer
                .compiler_data
                .const_pool
                .name_pool
                .get(ty_name)
                .unwrap(),
        )
    }

    fn convert_to_vm_type(&self, ty: usize) -> VmStackType {
        if ty == self.get_type_id("int") {
            VmStackType::Int
        } else if ty == self.get_type_id("float") {
            VmStackType::Float
        } else if ty == self.get_type_id("str") {
            VmStackType::Str
        } else if ty == self.get_type_id("char") {
            VmStackType::Char
        } else if ty == self.get_type_id("bool") {
            VmStackType::Bool
        } else {
            VmStackType::Object
        }
    }

    fn move_val_into_obj_stack(&mut self) {
        let obj_top = self.process_info.stack_type.pop().unwrap();
        match obj_top {
            VmStackType::Int => self.add_bycode(Opcode::MoveInt, NO_ARG),
            VmStackType::Float => self.add_bycode(Opcode::MoveFloat, NO_ARG),
            VmStackType::Str => self.add_bycode(Opcode::MoveStr, NO_ARG),
            VmStackType::Char => self.add_bycode(Opcode::MoveChar, NO_ARG),
            VmStackType::Bool => self.add_bycode(Opcode::MoveBool, NO_ARG),
            VmStackType::Object => {}
        }
        self.process_info.new_type(VmStackType::Object);
    }

    fn val(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        let t = self.token_lexer.next_token()?;
        if t.tp == TokenType::ID {
            let token_data = t.data.unwrap();
            let idx = self.self_scope.as_ref().borrow().get_sym_idx(token_data);
            if idx.is_none() {
                self.try_err(
                    istry,
                    ErrorInfo::new(
                        t!(
                            SYMBOL_NOT_FOUND,
                            "0" = self.token_lexer.compiler_data.const_pool.id_name[token_data]
                        ),
                        t!(SYMBOL_ERROR),
                    ),
                )?
            }
            let idx = idx.unwrap();
            let nxt = self.token_lexer.next_token()?;
            if nxt.tp == TokenType::LeftSmallBrace {
                let func_obj = self.self_scope.as_ref().borrow().get_function(idx).unwrap();
                let argv_list = self.opt_args(&func_obj)?;
                // match )
                self.check_next_token(TokenType::RightSmallBrace)?;
                // 阐明此处设计，首先我们的函数模板会以any的方式来占位，接下来调用的时候有几种情况，第一种就是入参有any，这种情况下我们会保留一份虚函数调用版本
                // 第二种情况就是入参有明确的类型
                // 接下来在这种情况的基础上再分两种情况
                // 第一种情况是自定义函数，这种情况下我们会像cpp模板那样对应生成版本
                // 第二种情况是rust函数，这种情况下我们只能记录类型，然后由rust函数自己判断从哪个栈中取出函数
                // 还有一种情况是any类型传入到函数中
                // 这种情况无论是哪种函数我们都会插入一条尝试转换类型的指令,将类型栈进行移动
                // 但是仅仅提供将其它类型移动到TrcObj的指令和从TrcObj转换到类型栈的函数
                // 类型之间会互相转换我们会以内置函数形式提供
                // 这是为了加速
                // 可变参数的话，因为类型不确定，我们会将其生成指令移入obj栈中
                if let Err(e) = func_obj.get_io().check_argvs(argv_list) {
                    self.try_err(istry, e)?
                }
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
            self.try_err(
                istry,
                ErrorInfo::new(t!(UNEXPECTED_TOKEN, "0" = t.tp), t!(SYNTAX_ERROR)),
            )?
        }
    }

    fn item(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        if let Ok(v) = self.val(true) {
            return Ok(v);
        }
        let t = self.token_lexer.next_token()?;
        match t.tp {
            TokenType::IntValue => {
                self.add_bycode(Opcode::LoadInt, t.data.unwrap());
                self.process_info.new_type(VmStackType::Int);
                Ok(TypeAllowNull::Yes(self.get_type_id("int")))
            }
            TokenType::FloatValue => {
                self.add_bycode(Opcode::LoadFloat, t.data.unwrap());
                self.process_info.new_type(VmStackType::Float);
                Ok(TypeAllowNull::Yes(self.get_type_id("float")))
            }
            TokenType::StringValue => {
                self.add_bycode(Opcode::LoadString, t.data.unwrap());
                self.process_info.new_type(VmStackType::Str);
                Ok(TypeAllowNull::Yes(self.get_type_id("str")))
            }
            TokenType::CharValue => {
                self.add_bycode(Opcode::LoadChar, t.data.unwrap());
                self.process_info.new_type(VmStackType::Char);
                Ok(TypeAllowNull::Yes(self.get_type_id("char")))
            }
            TokenType::BoolValue => {
                self.add_bycode(Opcode::LoadBool, t.data.unwrap());
                self.process_info.new_type(VmStackType::Bool);
                Ok(TypeAllowNull::Yes(self.get_type_id("bool")))
            }
            _ => {
                self.token_lexer.next_back(t.clone());
                self.try_err(
                    istry,
                    ErrorInfo::new(t!(UNEXPECTED_TOKEN, "0" = t.tp), t!(SYNTAX_ERROR)),
                )?
            }
        }
    }

    fn unary_opcode_impl(
        &mut self,
        istry: bool,
        optoken: TokenType,
        valtype: usize,
    ) -> AstError<TypeAllowNull> {
        let class_obj = self
            .self_scope
            .as_ref()
            .borrow()
            .get_class(valtype)
            .unwrap();
        let oride = class_obj.get_override_func(optoken.clone());
        match oride {
            Some(v) => {
                let tmp = v.io.check_argvs(vec![]);
                match tmp {
                    Ok(_) => {
                        self.add_bycode(v.opcode.clone(), NO_ARG);
                        Ok(v.io.return_type.clone())
                    }
                    Err(e) => self.try_err(istry, e)?,
                }
            }
            None => self.try_err(
                istry,
                ErrorInfo::new(
                    t!(
                        OPERATOR_IS_NOT_SUPPORT,
                        "0" = optoken,
                        "1" = class_obj.get_name()
                    ),
                    t!(OPERATOR_ERROR),
                ),
            )?,
        }
    }

    fn factor(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        let next_token = self.token_lexer.next_token()?;
        match next_token.tp {
            TokenType::Sub => {
                let ret = self.factor(istry)?.unwrap();
                self.unary_opcode_impl(istry, TokenType::SelfNegative, ret)
            }
            TokenType::BitNot => {
                let ret = self.factor(istry)?.unwrap();
                self.unary_opcode_impl(istry, TokenType::BitNot, ret)
            }
            TokenType::Not => {
                let ret = self.factor(istry)?.unwrap();
                self.unary_opcode_impl(istry, TokenType::Not, ret)
            }
            TokenType::Add => self.factor(istry),
            TokenType::LeftSmallBrace => {
                let ret = self.expr(istry)?;
                self.check_next_token(TokenType::RightSmallBrace)?;
                Ok(ret)
            }
            _ => {
                self.token_lexer.next_back(next_token);
                Ok(self.item(istry)?)
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
        let path = self.token_lexer.next_token()?;
        if path.tp != TokenType::StringValue {
            self.try_err(
                istry,
                ErrorInfo::new(t!(UNEXPECTED_TOKEN, "0" = path.tp), t!(SYNTAX_ERROR)),
            )?
        }
        let path = std::path::PathBuf::from(
            self.token_lexer.compiler_data.const_pool.id_str[path.data.unwrap()]
                .clone()
                .replace('.', "/"),
        );
        // the standard library first
        let strpath = path.to_str().unwrap();
        if strpath.get(0..3) == Some("std") {
        } else if let InputSource::File(now_module_path) =
            self.token_lexer.compiler_data.option.inputsource.clone()
        {
            let mut now_module_path = std::path::PathBuf::from(now_module_path);
            now_module_path.pop();
            now_module_path = now_module_path.join(path);
            if now_module_path.exists() {}
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
                        if var.is_none() {
                            return Err(RuntimeError::new(
                                Box::new(self.token_lexer.compiler_data.content.clone()),
                                ErrorInfo::new(
                                    t!(
                                        SYMBOL_NOT_FOUND,
                                        "0" =
                                            self.token_lexer.compiler_data.const_pool.id_name[name]
                                    ),
                                    t!(SYMBOL_ERROR),
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
                                    t!(
                                        SYMBOL_REDEFINED,
                                        "0" =
                                            self.token_lexer.compiler_data.const_pool.id_name[name]
                                    ),
                                    t!(SYMBOL_ERROR),
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
        Ok(())
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
    use crate::compiler::{Compiler, INT_VAL_POOL_ONE, INT_VAL_POOL_ZERO};

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
                Inst::new(Opcode::BitNotInt, NO_ARG),
                Inst::new(Opcode::AddInt, NO_ARG)
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
                Inst::new(Opcode::SubInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::SubInt, NO_ARG)
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
                Inst::new(Opcode::SubInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(Opcode::MulInt, NO_ARG)
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
                Inst::new(Opcode::AddInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(Opcode::LoadInt, 5),
                Inst::new(Opcode::PowerInt, NO_ARG),
                Inst::new(Opcode::MulInt, NO_ARG),
                Inst::new(Opcode::SubInt, NO_ARG),
            ]
        );
    }

    #[test]
    fn test_expr_final() {
        gen_test_env!(r#"(1+-2)*3//4**(5**6)==1||7==(8&9)"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::SelfNegativeInt, NO_ARG),
                Inst::new(Opcode::AddInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::MulInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(Opcode::LoadInt, 5),
                Inst::new(Opcode::LoadInt, 6),
                Inst::new(Opcode::PowerInt, NO_ARG),
                Inst::new(Opcode::PowerInt, NO_ARG),
                Inst::new(Opcode::ExactDivInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::EqInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 7),
                Inst::new(Opcode::LoadInt, 8),
                Inst::new(Opcode::LoadInt, 9),
                Inst::new(Opcode::BitAndInt, NO_ARG),
                Inst::new(Opcode::EqInt, NO_ARG),
                Inst::new(Opcode::OrBool, NO_ARG),
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
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
                Inst::new(
                    Opcode::CallNative,
                    get_stdlib()
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

    #[test]
    fn test_if() {
        gen_test_env!(
            r#"a:=9 
if a<8{

} else if a>11 {

} else {
if 8 == 7 {

} else {

}
}"#,
            t
        );
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 9),
                Inst::new(Opcode::LoadInt, 8),
                Inst::new(Opcode::LtInt, NO_ARG),
                Inst::new(Opcode::JumpIfFalse, 0),
                Inst::new(Opcode::LoadInt, 0),
                Inst::new(Opcode::Jump, 0),
                Inst::new(Opcode::LoadInt, 0),
                Inst::new(Opcode::LoadInt, 11),
                Inst::new(Opcode::GtInt, NO_ARG),
                Inst::new(Opcode::JumpIfFalse, 0),
                Inst::new(Opcode::LoadInt, 0),
                Inst::new(Opcode::Jump, 0),
                Inst::new(Opcode::LoadInt, 0),
                Inst::new(Opcode::LoadInt, 7)
            ]
        )
    }

    #[test]
    fn test_var_params() {
        gen_test_env!(r#"print("{}{}{}", 1, 2, 3)"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(
                    Opcode::CallNative,
                    get_stdlib()
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
}
