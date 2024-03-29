mod ast_base;
mod lexprocess;

use crate::base::{
    codegen::{Opcode, StaticData, VmStackType, ARG_WRONG, NO_ARG},
    error::*,
    func,
    stdlib::{get_stdlib, ArgsNameTy, IOType, RustFunction, BOOL, CHAR, FLOAT, INT, STR},
};
use crate::compiler::token::TokenType::RightBigBrace;
use rust_i18n::t;
use std::borrow::{Borrow, BorrowMut};
use std::mem::swap;
use std::{cell::RefCell, rc::Rc};

use super::{
    scope::*,
    token::{ConstPoolIndexTy, TokenType},
    InputSource, TokenLex,
};
use crate::base::stdlib::FunctionInterface;

#[derive(Default)]
struct Cache {
    pub(crate) intty_id: TyIdxTy,
    pub(crate) floatty_id: TyIdxTy,
    pub(crate) charty_id: TyIdxTy,
    pub(crate) boolty_id: TyIdxTy,
    pub(crate) strty_id: TyIdxTy,
}

impl Cache {
    pub fn new() -> Self {
        Self {
            ..Default::default()
        }
    }
}

pub struct AstBuilder<'a> {
    pub token_lexer: TokenLex<'a>,
    pub staticdata: StaticData,
    self_scope: Rc<RefCell<SymScope>>,
    process_info: lexprocess::LexProcess,
    cache: Cache,
    // record if the fisrt func is defined
    first_func: bool,
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
                    self.process_info.cal_val(stage_ty);
                    match tyb {
                        TypeAllowNull::No => {
                            Ok(TypeAllowNull::Yes(stage_ty))
                        }
                        TypeAllowNull::Yes(_) => {
                            Ok(tyb)
                        }
                    }
                })*
                _ => {
                    self.token_lexer.next_back(next_sym);
                    Ok(TypeAllowNull::No)
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
    pub fn new(mut token_lexer: TokenLex<'a>) -> Self {
        let prelude = get_stdlib().sub_modules.get("prelude").unwrap();
        for i in &prelude.functions {
            token_lexer.const_pool.add_id(i.0.clone());
        }
        for i in &prelude.classes {
            token_lexer.const_pool.add_id(i.0.clone());
        }
        let root_scope = Rc::new(RefCell::new(SymScope::new(None)));
        // 为root scope添加prelude
        let optimize = token_lexer.compiler_data.option.optimize;
        root_scope
            .as_ref()
            .borrow_mut()
            .import_prelude(&token_lexer.const_pool);
        let mut cache = Cache::new();
        let val_pool_ref = &token_lexer.const_pool;
        cache.intty_id = Self::get_type_id_internel(root_scope.clone(), val_pool_ref, INT).unwrap();
        cache.floatty_id =
            Self::get_type_id_internel(root_scope.clone(), val_pool_ref, FLOAT).unwrap();
        cache.charty_id =
            Self::get_type_id_internel(root_scope.clone(), val_pool_ref, CHAR).unwrap();
        cache.strty_id = Self::get_type_id_internel(root_scope.clone(), val_pool_ref, STR).unwrap();
        cache.boolty_id =
            Self::get_type_id_internel(root_scope.clone(), val_pool_ref, BOOL).unwrap();
        AstBuilder {
            token_lexer,
            staticdata: StaticData::new(),
            self_scope: root_scope,
            process_info: lexprocess::LexProcess::new(),
            cache,
            first_func: false,
        }
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

    pub fn prepare_get_static(&mut self) -> &StaticData {
        self.staticdata.constpool = self.token_lexer.const_pool.store_val_to_vm();
        &self.staticdata
    }

    pub fn return_static_data(mut self) -> StaticData {
        self.prepare_get_static();
        self.staticdata
    }

    fn while_lex(&mut self) -> AstError<()> {
        let mut prev_loop_state = true;
        swap(
            &mut prev_loop_state,
            &mut self.self_scope.as_ref().borrow_mut().in_loop,
        );
        let condit_id = self.staticdata.get_next_opcode_id();
        self.lex_condit()?;
        self.get_token_checked(TokenType::LeftBigBrace)?;
        self.add_bycode(Opcode::JumpIfFalse, ARG_WRONG);
        let jump_false_id = self.staticdata.get_last_opcode_id();
        self.lex_until(RightBigBrace)?;
        self.add_bycode(Opcode::Jump, condit_id);
        let opcode_after_while = self.staticdata.get_next_opcode_id();
        self.staticdata.inst[jump_false_id].operand = opcode_after_while;
        let mut break_record = vec![];
        swap(
            &mut break_record,
            &mut self.self_scope.as_ref().borrow_mut().for_break,
        );
        for i in break_record {
            self.staticdata.inst[i].operand = opcode_after_while;
        }
        let mut continue_record = vec![];
        swap(
            &mut continue_record,
            &mut self.self_scope.as_ref().borrow_mut().for_continue,
        );
        for i in continue_record {
            self.staticdata.inst[i].operand = condit_id;
        }
        swap(
            &mut prev_loop_state,
            &mut self.self_scope.as_ref().borrow_mut().in_loop,
        );
        Ok(())
    }

    fn for_lex(&mut self) -> AstError<()> {
        // init
        let mut prev_loop_state = true;
        swap(
            &mut prev_loop_state,
            &mut self.self_scope.as_ref().borrow_mut().in_loop,
        );
        self.statement()?;
        self.get_token_checked(TokenType::Semicolon)?;
        // condit
        let conid_id = self.staticdata.get_next_opcode_id();
        self.lex_condit()?;
        self.get_token_checked(TokenType::Semicolon)?;
        // 记录当前的头
        // action
        let mut token_save = vec![];
        loop {
            let t = self.token_lexer.next_token()?;
            if t.tp == TokenType::LeftBigBrace {
                break;
            }
            if t.tp == TokenType::EndOfFile {
                self.token_lexer
                    .compiler_data
                    .report_compiler_error(ErrorInfo::new(
                        t!(UNEXPECTED_TOKEN, "0" = t.tp),
                        t!(SYNTAX_ERROR),
                    ))?;
            }
            token_save.push(t);
        }
        // loop
        self.add_bycode(Opcode::JumpIfFalse, ARG_WRONG);
        // 这里是条件判断，是否越出循环
        let jump_false_id = self.staticdata.get_last_opcode_id();
        // 解析循环体
        self.lex_until(RightBigBrace)?;
        // 将先前储存的循环控制语句恢复
        token_save.reverse();
        for i in token_save {
            self.token_lexer.next_back(i);
        }
        let opcode_goto = self.staticdata.get_next_opcode_id();
        // 解析循环控制语句
        self.statement()?;
        // 跳转到条件判断语句
        self.add_bycode(Opcode::Jump, conid_id);
        let next_opcode_after_for = self.staticdata.get_next_opcode_id();
        self.staticdata.inst[jump_false_id].operand = next_opcode_after_for;
        // 开始处理所有的break
        let mut break_record = vec![];
        swap(
            &mut break_record,
            &mut self.self_scope.as_ref().borrow_mut().for_break,
        );
        for i in break_record {
            self.staticdata.inst[i].operand = next_opcode_after_for;
        }
        let mut continue_record = vec![];
        swap(
            &mut continue_record,
            &mut self.self_scope.as_ref().borrow_mut().for_continue,
        );
        for i in continue_record {
            self.staticdata.inst[i].operand = opcode_goto;
        }
        // 重置循环状态
        swap(
            &mut prev_loop_state,
            &mut self.self_scope.as_ref().borrow_mut().in_loop,
        );
        Ok(())
    }

    fn add_var_params_bycode(&mut self, var_params_num: usize) {
        let tmp = self.token_lexer.const_pool.add_int(var_params_num as i64);
        self.add_bycode(Opcode::LoadInt, tmp);
    }

    /// 解析出函数参数
    fn opt_args(&mut self, lex_func_obj: &Func) -> AstError<Vec<usize>> {
        let mut ret = vec![];
        let mut var_params_num = 0;
        let io_tmp = lex_func_obj.get_io();
        loop {
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
                        self.process_info.pop_last_ty();
                        var_params_num += 1;
                    } else {
                        ret.push(t)
                    }
                }
            }
        }
    }

    fn move_val_into_obj_stack(&mut self) {
        let obj_top = self.process_info.stack_type.last().copied().unwrap();
        match self.convert_id_to_vm_ty(obj_top) {
            VmStackType::Int => self.add_bycode(Opcode::MoveInt, NO_ARG),
            VmStackType::Float => self.add_bycode(Opcode::MoveFloat, NO_ARG),
            VmStackType::Str => self.add_bycode(Opcode::MoveStr, NO_ARG),
            VmStackType::Char => self.add_bycode(Opcode::MoveChar, NO_ARG),
            VmStackType::Bool => self.add_bycode(Opcode::MoveBool, NO_ARG),
            VmStackType::Object => {}
        }
    }

    fn load_var(&mut self, ty: TyIdxTy) -> Opcode {
        if self.process_info.is_global {
            match self.convert_id_to_vm_ty(ty) {
                VmStackType::Int => Opcode::LoadGlobalVarInt,
                VmStackType::Float => Opcode::LoadGlobalVarFloat,
                VmStackType::Str => Opcode::LoadGlobalVarStr,
                VmStackType::Char => Opcode::LoadGlobalVarChar,
                VmStackType::Bool => Opcode::LoadGlobalVarBool,
                VmStackType::Object => Opcode::LoadGlobalVarObj,
            }
        } else {
            match self.convert_id_to_vm_ty(ty) {
                VmStackType::Int => Opcode::LoadLocalVarInt,
                VmStackType::Float => Opcode::LoadLocalVarFloat,
                VmStackType::Str => Opcode::LoadLocalVarStr,
                VmStackType::Char => Opcode::LoadLocalVarChar,
                VmStackType::Bool => Opcode::LoadLocalVarBool,
                VmStackType::Object => Opcode::LoadLocalVarObj,
            }
        }
    }

    /// 解析函数，变量等的读取
    fn val(&mut self, istry: bool) -> AstError<TypeAllowNull> {
        let t = self.token_lexer.next_token()?;
        if t.tp == TokenType::ID {
            let token_data = t.data.unwrap();
            let idx = self.self_scope.as_ref().borrow().get_sym(token_data);
            if idx.is_none() {
                self.try_err(
                    istry,
                    ErrorInfo::new(
                        t!(
                            SYMBOL_NOT_FOUND,
                            "0" = self.token_lexer.const_pool.id_name[token_data]
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
                self.get_token_checked(TokenType::RightSmallBrace)?;
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
                    let msg = self.gen_args_error(e);
                    self.try_err(istry, msg)?
                }
                if let Some(obj) = func_obj.downcast_ref::<RustFunction>() {
                    self.add_bycode(Opcode::CallNative, obj.buildin_id);
                } else if let Some(obj) = func_obj.downcast_ref::<CustomFunction>() {
                    self.add_bycode(Opcode::CallCustom, obj.custom_id);
                }
                Ok(func_obj.get_io().return_type.clone())
            } else {
                self.token_lexer.next_back(nxt);
                let var = match self.self_scope.as_ref().borrow().get_var(idx) {
                    None => self.try_err(
                        istry,
                        ErrorInfo::new(
                            t!(
                                SYMBOL_NOT_FOUND,
                                "0" = self.token_lexer.const_pool.id_name[token_data]
                            ),
                            t!(SYMBOL_ERROR),
                        ),
                    )?,
                    Some(v) => v,
                };
                let tmp = self.load_var(var.ty);
                self.add_bycode(tmp, var.addr);
                self.process_info.new_type(var.ty);
                Ok(TypeAllowNull::Yes(var.ty))
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
        let t = self.token_lexer.next_token()?;
        match t.tp {
            TokenType::IntValue => {
                self.add_bycode(Opcode::LoadInt, t.data.unwrap());
                self.process_info.new_type(self.cache.intty_id);
                Ok(TypeAllowNull::Yes(self.cache.intty_id))
            }
            TokenType::FloatValue => {
                self.add_bycode(Opcode::LoadFloat, t.data.unwrap());
                self.process_info.new_type(self.cache.floatty_id);
                Ok(TypeAllowNull::Yes(self.cache.floatty_id))
            }
            TokenType::StringValue => {
                self.add_bycode(Opcode::LoadString, t.data.unwrap());
                self.process_info.new_type(self.cache.strty_id);
                Ok(TypeAllowNull::Yes(self.cache.strty_id))
            }
            TokenType::CharValue => {
                self.add_bycode(Opcode::LoadChar, t.data.unwrap());
                self.process_info.new_type(self.cache.charty_id);
                Ok(TypeAllowNull::Yes(self.cache.charty_id))
            }
            TokenType::BoolValue => {
                self.add_bycode(Opcode::LoadBool, t.data.unwrap());
                self.process_info.new_type(self.cache.boolty_id);
                Ok(TypeAllowNull::Yes(self.cache.boolty_id))
            }
            _ => {
                self.token_lexer.next_back(t.clone());
                Ok(self.val(istry)?)
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
                    Err(e) => {
                        let msg = self.gen_args_error(e);
                        self.try_err(istry, msg)
                    }
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
                self.get_token_checked(TokenType::RightSmallBrace)?;
                Ok(ret)
            }
            _ => {
                self.token_lexer.next_back(next_token);
                Ok(self.item(istry)?)
            }
        }
    }

    fn def_func(&mut self) -> AstError<()> {
        let funcname = self.get_token_checked(TokenType::ID)?.data.unwrap();
        let name_id = match self.self_scope.as_ref().borrow_mut().insert_sym(funcname) {
            Some(v) => v,
            None => {
                return self.gen_error(ErrorInfo::new(
                    t!(
                        SYMBOL_REDEFINED,
                        "0" = self.token_lexer.const_pool.id_name[funcname]
                    ),
                    t!(SYMBOL_ERROR),
                ));
            }
        };
        // lex args
        self.get_token_checked(TokenType::LeftSmallBrace)?;
        let mut argname: ArgsNameTy = vec![];
        let mut ty_list: Vec<TyIdxTy> = vec![];
        loop {
            let t = self.token_lexer.next_token()?;
            if t.tp == TokenType::RightSmallBrace {
                break;
            }
            if t.tp != TokenType::Comma {
                self.token_lexer.next_back(t);
            }
            let name_id = self.get_token_checked(TokenType::ID)?.data.unwrap();
            argname.push(name_id);
            self.get_token_checked(TokenType::Colon)?;
            ty_list.push(self.get_ty(false)?);
        }
        // 返回值解析
        let return_ty = match self.get_ty(true) {
            Err(_) => TypeAllowNull::No,
            Ok(ty) => TypeAllowNull::Yes(ty),
        };
        let io = IOType::new(ty_list, return_ty, false);
        self.get_token_checked(TokenType::LeftBigBrace)?;
        let mut function_body = vec![];
        let mut cnt = 1;
        loop {
            let t = self.token_lexer.next_token()?;
            function_body.push((t.clone(), self.token_lexer.compiler_data.context.get_line()));
            if t.tp == RightBigBrace {
                cnt -= 1;
                if cnt == 0 {
                    break;
                }
            } else if t.tp == TokenType::LeftBigBrace {
                cnt += 1;
            }
        }
        // self.self_scope = tmp.clone();
        self.self_scope.as_ref().borrow_mut().add_custom_function(
            name_id,
            CustomFunction::new(
                io,
                argname,
                self.token_lexer.const_pool.id_name[funcname].clone(),
            ),
            function_body,
        );
        Ok(())
    }

    fn def_class(&mut self) -> AstError<()> {
        Ok(())
    }

    fn import_module(&mut self, istry: bool) -> AstError<()> {
        let tok = self
            .get_token_checked(TokenType::StringValue)?
            .data
            .unwrap();
        let mut path = self.token_lexer.const_pool.id_str[tok].clone();
        // the standard library first
        if path.starts_with("std") {
            // let mut import_item_name = String::new();
            // loop {
            //     let c = path.pop().unwrap();
            //     if c == '.' {
            //         break;
            //     }
            //     import_item_name = format!("{}{}", c, import_item_name);
            // }
            let mut items = path.split(".");
            // 删除std
            items.next();
            let now = match get_stdlib().get_module(items) {
                Some(d) => d,
                None => {
                    return self.try_err(
                        istry,
                        ErrorInfo::new(t!(SYMBOL_NOT_FOUND, "0" = path), t!(SYMBOL_ERROR)),
                    );
                }
            };
            self.import_module_sym(&now);
            self.self_scope.as_ref().borrow_mut().import_native_module(
                tok,
                &now,
                &self.token_lexer.const_pool,
            );
        } else if let InputSource::File(now_module_path) =
            self.token_lexer.compiler_data.option.inputsource.clone()
        {
            let path = std::path::PathBuf::from(path.replace('.', "/"));

            let mut now_module_path = std::path::PathBuf::from(now_module_path);
            now_module_path.pop();
            now_module_path = now_module_path.join(path);
            if now_module_path.exists() {}
        }
        Ok(())
    }

    /// 生成修改变量的指令
    fn modify_var(&mut self, varty: TyIdxTy, var_addr: usize, is_global: bool) {
        self.add_bycode(
            if !is_global {
                match self.convert_id_to_vm_ty(varty) {
                    VmStackType::Int => Opcode::StoreLocalInt,
                    VmStackType::Float => Opcode::StoreLocalFloat,
                    VmStackType::Str => Opcode::StoreLocalStr,
                    VmStackType::Char => Opcode::StoreLocalChar,
                    VmStackType::Bool => Opcode::StoreLocalBool,
                    VmStackType::Object => Opcode::StoreLocalObj,
                }
            } else {
                match self.convert_id_to_vm_ty(varty) {
                    VmStackType::Int => Opcode::StoreGlobalInt,
                    VmStackType::Float => Opcode::StoreGlobalFloat,
                    VmStackType::Str => Opcode::StoreGlobalStr,
                    VmStackType::Char => Opcode::StoreGlobalChar,
                    VmStackType::Bool => Opcode::StoreGlobalBool,
                    VmStackType::Object => Opcode::StoreGlobalObj,
                }
            },
            var_addr,
        );
    }

    /// 生成新建变量的指令
    fn new_var(&mut self, name: ConstPoolIndexTy, varty: ScopeAllocClassId) -> AstError<()> {
        let sym_idx = match self.self_scope.as_ref().borrow_mut().insert_sym(name) {
            Some(v) => v,
            None => {
                return self.gen_error(ErrorInfo::new(
                    t!(
                        SYMBOL_REDEFINED,
                        "0" = self.token_lexer.const_pool.id_name[name]
                    ),
                    t!(SYMBOL_ERROR),
                ));
            }
        };
        let (var_sym, var_addr) =
            self.self_scope
                .as_ref()
                .borrow_mut()
                .add_var(sym_idx, varty, self.get_ty_sz(varty));
        self.modify_var(varty, var_addr, self.process_info.is_global);
        self.staticdata
            .update_var_table_mem_sz(self.self_scope.as_ref().borrow().get_var_table_sz());
        Ok(())
    }

    fn store_var(&mut self, name: usize) -> RunResult<()> {
        self.expr(false)?;
        let var_type = match self.process_info.get_last_ty() {
            Some(v) => v,
            None => {
                return self.gen_error(ErrorInfo::new(t!(EXPECTED_EXPR), t!(SYNTAX_ERROR)));
            }
        };
        self.new_var(name, var_type)?;
        Ok(())
    }

    fn assign_var(&mut self, name: usize) -> RunResult<()> {
        self.expr(false)?;
        let var_type = match self.process_info.get_last_ty() {
            Some(v) => v,
            None => {
                return self.gen_error(ErrorInfo::new(t!(EXPECTED_EXPR), t!(SYNTAX_ERROR)));
            }
        };
        let var = match self.self_scope.as_ref().borrow().get_var(name) {
            Some(v) => v,
            None => {
                return self.gen_error(ErrorInfo::new(
                    t!(
                        SYMBOL_NOT_FOUND,
                        "0" = self.token_lexer.const_pool.id_name[name]
                    ),
                    t!(SYMBOL_ERROR),
                ))
            }
        };
        if var.ty != var_type {
            return self.gen_error(ErrorInfo::new(t!(TYPE_NOT_THE_SAME), t!(TYPE_ERROR)));
        }
        self.modify_var(var.ty, var.addr, self.process_info.is_global);
        Ok(())
    }

    fn lex_until(&mut self, end_state: TokenType) -> RunResult<()> {
        loop {
            let t = self.token_lexer.next_token()?;
            if t.tp == end_state {
                break;
            }
            self.token_lexer.next_back(t);
            self.statement()?;
        }
        Ok(())
    }

    fn lex_condit(&mut self) -> RunResult<()> {
        self.expr(false)?;
        match self.process_info.stack_type.last() {
            None => {
                return self.gen_error(ErrorInfo::new(t!(EXPECTED_EXPR), t!(SYNTAX_ERROR)));
            }
            Some(ty) => {
                if *ty != self.cache.boolty_id {
                    return self.gen_error(ErrorInfo::new(t!(JUST_ACCEPT_BOOL), t!(TYPE_ERROR)));
                }
            }
        }
        Ok(())
    }

    fn if_lex(&mut self) -> RunResult<()> {
        self.lex_condit()?;
        self.get_token_checked(TokenType::LeftBigBrace)?;
        // 最后需要跳转地址
        let mut save_jump_opcode_idx = vec![];
        loop {
            let op_idx = self.staticdata.inst.len();
            // 本行是为了跳转到下一个分支
            self.add_bycode(Opcode::JumpIfFalse, ARG_WRONG);
            self.lex_until(RightBigBrace)?;
            self.staticdata.inst[op_idx].operand = self.staticdata.get_next_opcode_id();
            self.add_bycode(Opcode::Jump, ARG_WRONG);
            save_jump_opcode_idx.push(self.staticdata.get_last_opcode_id());
            let t = self.token_lexer.next_token()?;
            if t.tp == TokenType::Else {
                let nxt_tok = self.token_lexer.next_token()?;
                if nxt_tok.tp == TokenType::If {
                    self.lex_condit()?;
                    self.get_token_checked(TokenType::LeftBigBrace)?;
                    continue;
                }
                self.token_lexer.next_back(nxt_tok);
                self.get_token_checked(TokenType::LeftBigBrace)?;
                self.lex_until(RightBigBrace)?;
                break;
            }
            save_jump_opcode_idx.pop();
            self.del_opcode().unwrap();
            self.token_lexer.next_back(t);
            break;
        }
        for i in save_jump_opcode_idx {
            self.staticdata.inst[i].operand = self.staticdata.get_next_opcode_id();
        }
        Ok(())
    }

    fn statement(&mut self) -> RunResult<()> {
        let t = self.token_lexer.next_token()?;
        match t.tp {
            TokenType::Continue => {
                if !self.self_scope.as_ref().borrow().in_loop {
                    return self.gen_error(ErrorInfo::new(
                        t!(SHOULD_IN_LOOP, "0" = "continue"),
                        t!(SYNTAX_ERROR),
                    ));
                }
                self.add_bycode(Opcode::Jump, ARG_WRONG);
                self.self_scope
                    .as_ref()
                    .borrow_mut()
                    .for_continue
                    .push(self.staticdata.get_last_opcode_id());
                return Ok(());
            }
            TokenType::Break => {
                if !self.self_scope.as_ref().borrow().in_loop {
                    return self.gen_error(ErrorInfo::new(
                        t!(SHOULD_IN_LOOP, "0" = "break"),
                        t!(SYNTAX_ERROR),
                    ));
                }
                self.add_bycode(Opcode::Jump, ARG_WRONG);
                self.self_scope
                    .as_ref()
                    .borrow_mut()
                    .for_break
                    .push(self.staticdata.get_last_opcode_id());
                return Ok(());
            }
            TokenType::Return => {
                if self.process_info.is_global {
                    return self.gen_error(ErrorInfo::new(
                        t!(RETURN_SHOULD_IN_FUNCTION),
                        t!(SYNTAX_ERROR),
                    ));
                }
                // ignore the result
                // for return and return expr both
                let ret_type = self
                    .self_scope
                    .as_ref()
                    .borrow()
                    .func_io
                    .clone()
                    .unwrap()
                    .clone();
                match ret_type {
                    TypeAllowNull::Yes(ty) => {
                        self.expr(true)?;
                        let actual_ty = self.process_info.get_last_ty().unwrap();
                        if ty != actual_ty {
                            let s1 = self.get_ty_name(ty);
                            let s2 = self.get_ty_name(actual_ty);
                            return self.gen_error(ErrorInfo::new(
                                t!(RETURN_TYPE_ERROR, "0" = s1, "1" = s2),
                                t!(TYPE_ERROR),
                            ));
                        }
                    }
                    TypeAllowNull::No => {
                        if self.expr(true).is_ok() {
                            let actual_ty = self.process_info.get_last_ty().unwrap();
                            let name = self.get_ty_name(actual_ty);
                            return self.gen_error(ErrorInfo::new(
                                t!(RETURN_TYPE_ERROR, "0" = "void", "1" = name),
                                t!(TYPE_ERROR),
                            ));
                        }
                    }
                }
                self.add_bycode(Opcode::PopFrame, NO_ARG);
                return Ok(());
            }
            TokenType::Func => {
                self.def_func()?;
                return Ok(());
            }
            TokenType::Class => {
                self.def_class()?;
                return Ok(());
            }
            TokenType::While => {
                self.while_lex()?;
                return Ok(());
            }
            TokenType::For => {
                self.for_lex()?;
                return Ok(());
            }
            TokenType::If => {
                self.if_lex()?;
                return Ok(());
            }
            TokenType::Import => {
                self.import_module(false)?;
                return Ok(());
            }
            TokenType::ID => {
                let name = t.data.unwrap();
                let tt = self.token_lexer.next_token()?;
                match tt.tp {
                    TokenType::Assign => {
                        self.assign_var(name)?;
                        return Ok(());
                    }
                    TokenType::Store => {
                        self.store_var(name)?;
                        return Ok(());
                    }
                    TokenType::SelfAdd => {}
                    TokenType::SelfSub => {}
                    TokenType::SelfMul => {}
                    TokenType::SelfDiv => {}
                    TokenType::SelfMod => {}
                    TokenType::SelfOr => {}
                    TokenType::SelfAnd => {}
                    _ => {
                        self.token_lexer.next_back(tt);
                    }
                }
            }
            _ => {}
        }
        self.token_lexer.next_back(t);
        self.expr(false)?;
        Ok(())
    }

    /// # Return
    /// 返回函数的首地址
    fn lex_function(&mut self, funcid: usize, body: &FuncBodyTy) -> RunResult<(usize, usize)> {
        if !self.first_func {
            // 如果不是第一个函数，在末尾加上结束主程序的指令
            self.first_func = true;
            self.add_bycode(Opcode::Stop, NO_ARG);
            self.staticdata.function_split = Some(self.staticdata.get_last_opcode_id());
        }
        let begin_inst_idx = self.staticdata.get_next_opcode_id();
        let func_obj = self
            .self_scope
            .as_ref()
            .borrow()
            .get_function(funcid)
            .unwrap()
            .downcast::<CustomFunction>()
            .expect("Expect Custom Function");
        let io = func_obj.get_io();
        let tmp = self.self_scope.clone();
        // 解析参数
        self.self_scope = Rc::new(RefCell::new(SymScope::new(Some(tmp.clone()))));
        self.self_scope.as_ref().borrow_mut().func_io = Some(io.return_type.clone());
        debug_assert_eq!(io.argvs_type.len(), func_obj.args_names.len());
        for (argty, argname) in io
            .argvs_type
            .iter()
            .rev()
            .zip(func_obj.args_names.iter().rev())
        {
            self.new_var(*argname, *argty).unwrap();
        }
        // 回退函数体
        for i in body.iter().rev() {
            self.token_lexer.next_back_with_line(i.0.clone(), i.1);
        }
        loop {
            let t = self.token_lexer.next_token()?;
            if t.tp == RightBigBrace {
                break;
            }
            self.token_lexer.next_back(t);
            self.statement()?;
        }
        if !self.staticdata.inst.is_empty()
            && self.staticdata.inst.last().unwrap().opcode != Opcode::PopFrame
        {
            self.add_bycode(Opcode::PopFrame, NO_ARG);
        }
        self.generate_func_in_scope()?;
        let mem_sz = self.self_scope.as_ref().borrow().get_var_table_sz();
        self.self_scope = tmp.clone();
        Ok((begin_inst_idx, mem_sz))
    }

    fn generate_func_in_scope(&mut self) -> AstError<()> {
        let mut tmp = vec![];
        std::mem::swap(
            &mut tmp,
            &mut self.self_scope.as_ref().borrow_mut().funcs_temp_store,
        );
        for i in tmp {
            let (code_begin, var_mem_sz) = self.lex_function(i.0, &i.1)?;
            self.staticdata
                .funcs
                .push(func::Func::new(code_begin, var_mem_sz))
        }
        Ok(())
    }

    pub fn generate_code(&mut self) -> RunResult<()> {
        self.process_info.is_global = true;
        self.lex_until(TokenType::EndOfFile)?;
        // 结束一个作用域的代码解析后再解析这里面的函数
        self.process_info.is_global = false;
        self.generate_func_in_scope()?;
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use std::mem::size_of;

    use crate::{
        base::{codegen::Inst, stdlib::get_prelude_function},
        compiler::*,
    };

    use super::*;

    macro_rules! gen_test_env {
        ($test_code:expr, $env_name:ident) => {
            use crate::compiler::InputSource;
            use crate::compiler::Option;
            let mut compiler = Compiler::new_string_compiler(
                Option::new(false, InputSource::StringInternal),
                $test_code,
            );
            let token_lexer = TokenLex::new(&mut compiler);
            let mut $env_name = AstBuilder::new(token_lexer);
        };
    }

    /// 前面有int_nums个int时的首地址
    fn get_offset(int_nums: usize) -> usize {
        size_of::<i64>() * int_nums
    }

    #[test]
    fn test_assign() {
        gen_test_env!(
            r#"a:=10
        a=10
        b:=90
        print("{}{}", a, b)"#,
            t
        );
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::StoreGlobalInt, get_offset(1)),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadGlobalVarInt, get_offset(1)),
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
            ],
        )
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
    fn test_expr_in_arg() {
        gen_test_env!(
            r#"
a:=90
print("{}", a+90)"#,
            t
        );
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::AddInt, NO_ARG),
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
            ]
        )
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
                    get_prelude_function("print").unwrap().buildin_id
                ),
            ]
        )
    }

    #[test]
    #[should_panic(expected = "OperatorError")]
    fn test_wrong_type() {
        gen_test_env!(r#"1.0+9"#, t);
        t.generate_code().unwrap();
        println!("{:?}", t.staticdata.inst);
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
    fn test_if_easy() {
        gen_test_env!(r#"if 1==1 { print("hello world") }"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::EqInt, NO_ARG),
                Inst::new(Opcode::JumpIfFalse, 7),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
            ]
        )
    }

    #[test]
    fn test_if_easy2() {
        gen_test_env!(
            r#"
        if 1==1 {
            print("hello world")
        } else if 1==2 {

        } else {
            print("hello world")
        }
        "#,
            t
        );
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
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::StoreGlobalInt, 0),
                Inst::new(Opcode::LoadGlobalVarInt, 0),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::LtInt, 0),
                Inst::new(Opcode::JumpIfFalse, 6),
                Inst::new(Opcode::Jump, 17),
                Inst::new(Opcode::LoadGlobalVarInt, 0),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(Opcode::GtInt, 0),
                Inst::new(Opcode::JumpIfFalse, 11),
                Inst::new(Opcode::Jump, 17),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::LoadInt, 5),
                Inst::new(Opcode::EqInt, 0),
                Inst::new(Opcode::JumpIfFalse, 16),
                Inst::new(Opcode::Jump, 17)
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
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
            ]
        )
    }

    #[test]
    fn test_while_1() {
        gen_test_env!(r#"while 1==1 { print("hello world") }"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::EqInt, NO_ARG),
                Inst::new(Opcode::JumpIfFalse, 8),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
                Inst::new(Opcode::Jump, 0)
            ]
        )
    }

    #[test]
    fn test_for_1() {
        gen_test_env!(r#"for i:=0; i<10; i=i+1 { print("hello world") }"#, t);
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 0),
                Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
                Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::LtInt, NO_ARG),
                Inst::new(Opcode::JumpIfFalse, 14),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
                Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::AddInt, NO_ARG),
                Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
                Inst::new(Opcode::Jump, 2)
            ]
        )
    }

    #[test]
    fn test_func_def_easy1() {
        gen_test_env!(
            r#"
func f1() {
    print("i am function that is called")
}
func f() {
    print("hello world")
    f1()
}"#,
            t
        );
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::Stop, NO_ARG),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
                Inst::new(Opcode::PopFrame, NO_ARG),
                Inst::new(Opcode::LoadString, 1),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
                Inst::new(Opcode::CallCustom, 0),
                Inst::new(Opcode::PopFrame, NO_ARG)
            ]
        )
    }

    #[test]
    fn test_call_custom_func_easy1() {
        gen_test_env!(
            r#"
func f() {
    print("hello world")
}
f()"#,
            t
        );
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::CallCustom, 0),
                Inst::new(Opcode::Stop, NO_ARG),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
                Inst::new(Opcode::PopFrame, NO_ARG)
            ]
        )
    }

    #[test]
    fn test_func_call_with_args() {
        gen_test_env!(
            r#"
func a1(a: int, b: int) {
    print("{}{}", a, b)
}
a1(0, 1)
a:=1
b:=0
print("{}{}", a, b)
        "#,
            t
        );
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 0),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::CallCustom, 0),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
                Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
                Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
                Inst::new(Opcode::StoreGlobalInt, get_offset(1)),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadGlobalVarInt, get_offset(1)),
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
                Inst::new(Opcode::Stop, NO_ARG),
                Inst::new(Opcode::StoreLocalInt, get_offset(0)),
                Inst::new(Opcode::StoreLocalInt, get_offset(1)),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadLocalVarInt, get_offset(1)),
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadLocalVarInt, get_offset(0)),
                Inst::new(Opcode::MoveInt, NO_ARG),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(
                    Opcode::CallNative,
                    get_prelude_function("print").unwrap().buildin_id
                ),
                Inst::new(Opcode::PopFrame, NO_ARG)
            ]
        )
    }

    #[test]
    fn test_for_break() {
        gen_test_env!(
            r#"for i:=0;i<=10;i=i+1{
  if i==3{
    continue
  }
  if i==5{
    break
  }
  println("{}", i)
}
a:=0
while a<10{
  a=a+1
  if a==3{
    continue
  }
  println("{}", a)
  if a==5{
    break
  }
}
"#,
            t
        );
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadInt, 0),
                Inst::new(Opcode::StoreGlobalInt, 0),
                Inst::new(Opcode::LoadGlobalVarInt, 0),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::LeInt, 0),
                Inst::new(Opcode::JumpIfFalse, 26),
                Inst::new(Opcode::LoadGlobalVarInt, 0),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::EqInt, 0),
                Inst::new(Opcode::JumpIfFalse, 11),
                Inst::new(Opcode::Jump, 21),
                Inst::new(Opcode::LoadGlobalVarInt, 0),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(Opcode::EqInt, 0),
                Inst::new(Opcode::JumpIfFalse, 16),
                Inst::new(Opcode::Jump, 26),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadGlobalVarInt, 0),
                Inst::new(Opcode::MoveInt, 0),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::CallNative, 1),
                Inst::new(Opcode::LoadGlobalVarInt, 0),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::AddInt, 0),
                Inst::new(Opcode::StoreGlobalInt, 0),
                Inst::new(Opcode::Jump, 2),
                Inst::new(Opcode::LoadInt, 0),
                Inst::new(Opcode::StoreGlobalInt, 8),
                Inst::new(Opcode::LoadGlobalVarInt, 8),
                Inst::new(Opcode::LoadInt, 2),
                Inst::new(Opcode::LtInt, 0),
                Inst::new(Opcode::JumpIfFalse, 52),
                Inst::new(Opcode::LoadGlobalVarInt, 8),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::AddInt, 0),
                Inst::new(Opcode::StoreGlobalInt, 8),
                Inst::new(Opcode::LoadGlobalVarInt, 8),
                Inst::new(Opcode::LoadInt, 3),
                Inst::new(Opcode::EqInt, 0),
                Inst::new(Opcode::JumpIfFalse, 41),
                Inst::new(Opcode::Jump, 28),
                Inst::new(Opcode::LoadString, 0),
                Inst::new(Opcode::LoadGlobalVarInt, 8),
                Inst::new(Opcode::MoveInt, 0),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::CallNative, 1),
                Inst::new(Opcode::LoadGlobalVarInt, 8),
                Inst::new(Opcode::LoadInt, 4),
                Inst::new(Opcode::EqInt, 0),
                Inst::new(Opcode::JumpIfFalse, 51),
                Inst::new(Opcode::Jump, 52),
                Inst::new(Opcode::Jump, 28),
            ]
        );
    }

    #[test]
    fn test_import_std() {
        gen_test_env!(
            r#"import "std.math"
print("{}", sin(9.8))
        "#,
            t
        );
        t.generate_code().unwrap();
        assert_eq!(
            t.staticdata.inst,
            vec![
                Inst::new(Opcode::LoadString, 1),
                Inst::new(Opcode::LoadFloat, 0),
                Inst::new(Opcode::CallNative, 3),
                Inst::new(Opcode::MoveFloat, 0),
                Inst::new(Opcode::LoadInt, 1),
                Inst::new(Opcode::CallNative, 0),
            ]
        );
    }
}
