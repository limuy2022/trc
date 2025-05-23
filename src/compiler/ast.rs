mod ast_base;
mod lexprocess;

use super::{InputSource, TokenLex, scope::*, token::Token};
use crate::{
    base::dll::load_module_storage,
    compiler::{
        CompilerImpl,
        manager::ModuleManager,
        token::{CONST_IDX_PLACEHOLDER, Token::RightBigBrace},
    },
};
use collection_literals::collection;
use libcore::*;
use rust_i18n::t;
use std::{
    cell::RefCell,
    collections::{HashMap, HashSet},
    mem::swap,
    path::PathBuf,
    rc::Rc,
};

#[derive(Default)]
struct Cache {
    pub(crate) intty_id: ClassIdxId,
    pub(crate) floatty_id: ClassIdxId,
    pub(crate) charty_id: ClassIdxId,
    pub(crate) boolty_id: ClassIdxId,
    pub(crate) strty_id: ClassIdxId,
}

impl Cache {
    pub fn new() -> Self {
        Self {
            ..Default::default()
        }
    }
}

pub struct ModuleUnit<'a> {
    pub token_lexer: Rc<RefCell<TokenLex<'a>>>,
    pub staticdata: StaticData,
    pub self_scope: Rc<RefCell<SymScope>>,
    process_info: lexprocess::LexProcess,
    cache: Cache,
    /// record if the first func is defined
    first_func: bool,
    /// 对哈希表去重，并且记录每个dll的函数起始索引
    modules_dll_dup: HashSet<String>,
    /// 每个模块的函数起始点
    modules_info: HashMap<String, usize>,
    /// 引用的dll
    modules_dll: Vec<String>,
    module_manager: Rc<RefCell<ModuleManager<'a>>>,
    compiler_data: Rc<RefCell<CompilerImpl>>,
}

type AstError<T> = RuntimeResult<T>;

macro_rules! tmp_expe_function_gen {
    ($tmpfuncname:ident, $next_item_func:ident, $($accepted_token:path),*) => {
        fn $tmpfuncname(&mut self, istry: bool, extend: ClassIdxId) -> AstError<()> {
            let next_sym = self.token_lexer.borrow_mut().next_token()?;
            match next_sym {
                $($accepted_token => {
                    self.$next_item_func(istry)?;
                    // 读取IOType检查
                    let func_obj = self.self_scope.borrow().get_class_by_class_id(extend).expect(&format!("Class {} not found", *extend));
                    let io_check = func_obj.get_override_func(match $accepted_token.convert_to_override() {
                        Some(v) => v,
                        None => {
                            panic!("error token {}", $accepted_token);
                        }
                    });
                    let io_check = match io_check {
                        None => self.try_err(istry,
                            ErrorInfo::new(
                                t!(OPERATOR_IS_NOT_SUPPORT, "0"=$accepted_token, "1"=func_obj.get_name()),
                                t!(OPERATOR_ERROR),
                            )
                        )?,
                        Some(v) => {
                            if v.io.check_argvs(vec![self.process_info.get_last_ty().expect("type stack is empty")]).is_err() {
                                self.try_err(istry,
                                    ErrorInfo::new(t!(OPERATOR_IS_NOT_SUPPORT, "0"=$accepted_token, "1"=func_obj.get_name()), t!(OPERATOR_ERROR)))?
                            }
                            v
                        }
                    };
                    self.add_bycode(io_check.opcode, NO_ARG);
                    let stage_ty = io_check.io.return_type.unwrap();
                    self.$tmpfuncname(istry, stage_ty)?;
                    self.process_info.cal_val(stage_ty);
                    match self.process_info.get_last_ty() {
                        None => {
                            self.process_info.new_type(stage_ty);
                        }
                        Some(v) => {
                            self.process_info.new_type(v);
                        }
                    }
                })*
                _ => {
                    self.token_lexer.borrow_mut().next_back(next_sym);
                }
            }
            Ok(())
        }
    };
}

/// there are a log of similar operators to be generated
macro_rules! expr_gen {
    ($funcname:ident, $next_item_func:ident, $($accepted_token:path),*) => {
        paste::paste!(
        tmp_expe_function_gen!([<_ $funcname>], $next_item_func, $($accepted_token),*);
        fn $funcname(&mut self, istry: bool) -> AstError<()> {
            self.$next_item_func(istry)?;
            match self.process_info.get_last_ty() {
                None => {}
                Some(v) => {
                    self.[<_ $funcname>](istry, v)?;
                }
            }
            Ok(())
        }
        );
    };
}

impl<'a> ModuleUnit<'a> {
    pub fn new(
        token_lexer: Rc<RefCell<TokenLex<'a>>>,
        compiler_data: Rc<RefCell<CompilerImpl>>,
        module_manager: Rc<RefCell<ModuleManager<'a>>>,
    ) -> Self {
        let root_scope = Rc::new(RefCell::new(SymScope::new(SymScopePrev::Root)));
        let stdlib_dll_name = libloading::library_filename("stdlib");
        let stdlib_dll =
            unsafe { libloading::Library::new(stdlib_dll_name.clone()).expect("without stdlib") };
        let (stdlib, stdstorage) = load_module_storage(&stdlib_dll);
        // 向token lexer常量池中添加prelude解析数据
        let prelude = stdlib.sub_modules().get("prelude").unwrap();
        for i in prelude.functions() {
            token_lexer.borrow_mut().add_id(i.0.clone());
        }
        for i in prelude.classes() {
            token_lexer.borrow_mut().add_id(i.0.clone());
        }
        // 为root scope添加prelude
        let _optimize = compiler_data.borrow_mut().option.optimize;
        let func_id_base = module_manager.borrow().global_extern_function_id();
        root_scope
            .borrow_mut()
            .import_native_module(
                prelude,
                stdstorage,
                token_lexer.borrow_mut().get_constpool(),
                func_id_base,
            )
            .expect("Import prelude but failed");
        root_scope.borrow_mut().add_imported_native_dll(
            "std".to_string(),
            Rc::new(stdlib_dll),
            stdstorage,
            |v| {
                module_manager.borrow_mut().add_extern_function_id(v);
            },
        );
        let mut cache = Cache::new();
        let tmp = token_lexer.clone();
        let tmp = tmp.borrow_mut();
        let val_pool_ref = &tmp.get_constpool();
        cache.intty_id = root_scope
            .borrow()
            .get_type_id_by_token(val_pool_ref.name_pool[INT])
            .unwrap();
        cache.floatty_id = root_scope
            .borrow()
            .get_type_id_by_token(val_pool_ref.name_pool[FLOAT])
            .unwrap();
        cache.charty_id = root_scope
            .borrow()
            .get_type_id_by_token(val_pool_ref.name_pool[CHAR])
            .unwrap();
        cache.strty_id = root_scope
            .borrow()
            .get_type_id_by_token(val_pool_ref.name_pool[STR])
            .unwrap();
        cache.boolty_id = root_scope
            .borrow()
            .get_type_id_by_token(val_pool_ref.name_pool[BOOL])
            .unwrap();
        // println!(
        //     "{} {} {} {} {}",
        //     cache.intty_id, cache.floatty_id, cache.charty_id, cache.strty_id, cache.boolty_id
        // );
        let name_str = stdlib_dll_name.to_str().unwrap().to_owned();
        ModuleUnit {
            token_lexer,
            staticdata: StaticData::new(),
            self_scope: root_scope,
            process_info: lexprocess::LexProcess::new(),
            cache,
            first_func: false,
            modules_dll_dup: collection! {name_str.clone()},
            modules_dll: vec![name_str.clone()],
            module_manager,
            compiler_data,
            modules_info: collection! {"std".to_owned() => 0},
        }
    }

    expr_gen!(expr9, factor, Token::Power);
    expr_gen!(
        expr8,
        expr9,
        Token::Mul,
        Token::Div,
        Token::Mod,
        Token::ExactDiv
    );
    expr_gen!(expr7, expr8, Token::Sub, Token::Add);
    expr_gen!(expr6, expr7, Token::BitLeftShift, Token::BitRightShift);
    expr_gen!(expr5, expr6, Token::BitAnd);
    expr_gen!(expr4, expr5, Token::Xor);
    expr_gen!(expr3, expr4, Token::BitOr);
    expr_gen!(
        expr2,
        expr3,
        Token::Equal,
        Token::NotEqual,
        Token::Less,
        Token::LessEqual,
        Token::Greater,
        Token::GreaterEqual
    );
    expr_gen!(expr1, expr2, Token::And);
    expr_gen!(expr, expr1, Token::Or);

    pub fn prepare_get_static(&mut self) -> &mut StaticData {
        self.staticdata.constpool = self
            .token_lexer
            .borrow_mut()
            .get_constpool_mut()
            .store_val_to_vm();
        self.staticdata.dll_module_should_loaded.clear();
        swap(
            &mut self.staticdata.dll_module_should_loaded,
            &mut self.modules_dll,
        );
        &mut self.staticdata
    }

    pub fn return_static_data(mut self) -> StaticData {
        self.prepare_get_static();
        self.staticdata
    }

    fn while_lex(&mut self) -> AstError<()> {
        let mut prev_loop_state = true;
        swap(
            &mut prev_loop_state,
            &mut self.self_scope.borrow_mut().in_loop,
        );
        let condit_id = self.staticdata.get_next_opcode_id();
        self.lex_condit()?;
        self.get_token_checked(Token::LeftBigBrace)?;
        self.add_bycode(Opcode::JumpIfFalse, ARG_WRONG);
        let jump_false_id = self.staticdata.get_last_opcode_id();
        self.lex_until(RightBigBrace)?;
        self.add_bycode(Opcode::Jump, condit_id);
        let opcode_after_while = self.staticdata.get_next_opcode_id();
        let idx: usize = jump_false_id.into();
        self.staticdata.inst[idx].operand.0 = opcode_after_while;
        let mut break_record = vec![];
        swap(
            &mut break_record,
            &mut self.self_scope.borrow_mut().for_break,
        );
        for i in break_record {
            self.staticdata.inst[i].operand.0 = opcode_after_while;
        }
        let mut continue_record = vec![];
        swap(
            &mut continue_record,
            &mut self.self_scope.borrow_mut().for_continue,
        );
        for i in continue_record {
            self.staticdata.inst[i].operand.0 = condit_id;
        }
        swap(
            &mut prev_loop_state,
            &mut self.self_scope.borrow_mut().in_loop,
        );
        Ok(())
    }

    fn for_lex(&mut self) -> AstError<()> {
        // init
        let mut prev_loop_state = true;
        swap(
            &mut prev_loop_state,
            &mut self.self_scope.borrow_mut().in_loop,
        );
        self.statement()?;
        self.get_token_checked(Token::Semicolon)?;
        // condit
        let conid_id = self.staticdata.get_next_opcode_id();
        self.lex_condit()?;
        self.get_token_checked(Token::Semicolon)?;
        // 记录当前的头
        // action
        let mut token_save = vec![];
        loop {
            let t = self.next_token()?;
            if t == Token::LeftBigBrace {
                break;
            }
            if t == Token::EndOfFile {
                self.compiler_data
                    .borrow_mut()
                    .report_compiler_error(ErrorInfo::new(
                        t!(UNEXPECTED_TOKEN, "0" = t),
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
            self.token_lexer.borrow_mut().next_back(i);
        }
        let opcode_goto = self.staticdata.get_next_opcode_id();
        // 解析循环控制语句
        self.statement()?;
        // 跳转到条件判断语句
        self.add_bycode(Opcode::Jump, conid_id);
        let next_opcode_after_for = self.staticdata.get_next_opcode_id();
        let idx: usize = jump_false_id.into();
        self.staticdata.inst[idx].operand.0 = next_opcode_after_for;
        // 开始处理所有的break
        let mut break_record = vec![];
        swap(
            &mut break_record,
            &mut self.self_scope.borrow_mut().for_break,
        );
        for i in break_record {
            self.staticdata.inst[i].operand.0 = next_opcode_after_for;
        }
        let mut continue_record = vec![];
        swap(
            &mut continue_record,
            &mut self.self_scope.borrow_mut().for_continue,
        );
        for i in continue_record {
            self.staticdata.inst[i].operand.0 = opcode_goto;
        }
        // 重置循环状态
        swap(
            &mut prev_loop_state,
            &mut self.self_scope.borrow_mut().in_loop,
        );
        Ok(())
    }

    fn add_var_params_bycode(&mut self, var_params_num: Opidx) {
        self.add_bycode(Opcode::LoadInt, var_params_num);
    }

    /// 解析出函数参数
    fn opt_args(&mut self, lex_func_obj: &Rc<dyn FunctionInterface>) -> AstError<Vec<ClassIdxId>> {
        let mut ret = vec![];
        let mut var_params_num = 0;
        let io_tmp = lex_func_obj.get_io();
        loop {
            let nextt = self.next_token()?;
            match nextt {
                Token::RightSmallBrace => {
                    if io_tmp.var_params {
                        self.add_var_params_bycode(Opidx(var_params_num as OpidxInternal));
                    }
                    self.token_lexer.borrow_mut().next_back(nextt);
                    return Ok(ret);
                }
                Token::Comma => {}
                _ => {
                    self.token_lexer.borrow_mut().next_back(nextt);
                }
            }
            self.expr(false)?;
            match self.process_info.get_last_ty() {
                None => {
                    self.gen_error(ErrorInfo::new(
                        t!(ARGUMENT_CANNOT_BE_VOID),
                        t!(ARGUMENT_ERROR),
                    ))?;
                }
                Some(t) => {
                    // 如果是可变参数是需要将其转入obj_stack的
                    if io_tmp.var_params && io_tmp.argvs_type.len() <= ret.len() {
                        // the values that have been stored is more than exact requirement of function
                        self.move_val_into_obj_stack();
                        var_params_num += 1;
                    } else {
                        ret.push(t)
                    }
                }
            }
        }
    }

    fn move_val_into_obj_stack(&mut self) {
        let obj_top = self.process_info.pop_last_ty().unwrap();
        match self.convert_id_to_vm_ty(obj_top) {
            VmStackType::Int => self.add_bycode(Opcode::MoveInt, NO_ARG),
            VmStackType::Float => self.add_bycode(Opcode::MoveFloat, NO_ARG),
            VmStackType::Str => self.add_bycode(Opcode::MoveStr, NO_ARG),
            VmStackType::Char => self.add_bycode(Opcode::MoveChar, NO_ARG),
            VmStackType::Bool => self.add_bycode(Opcode::MoveBool, NO_ARG),
            VmStackType::Object => {}
        }
    }

    fn load_var_opcode(&mut self, ty: ClassIdxId) -> (Opcode, usize) {
        let ret_opcode = if self.process_info.is_global {
            Opcode::LoadGlobalVar
        } else {
            Opcode::LoadLocalVar
        };
        (ret_opcode, self.get_ty_sz(ty))
    }

    fn load_var(
        &mut self,
        idx: ScopeAllocId,
        name_token: ConstPoolData,
        istry: bool,
    ) -> AstError<()> {
        let var = match self.self_scope.borrow().get_var(idx) {
            None => self.try_err(
                istry,
                ErrorInfo::new(
                    t!(
                        SYMBOL_NOT_FOUND,
                        "0" = self.token_lexer.borrow_mut().get_constpool().id_name[*name_token]
                    ),
                    t!(SYMBOL_ERROR),
                ),
            )?,
            Some(v) => v,
        };
        let tmp = self.load_var_opcode(var.ty);
        self.add_double_bycode(
            tmp.0,
            Opidx(var.addr as OpidxInternal),
            Opidx(tmp.1 as OpidxInternal),
        );
        self.process_info.new_type(var.ty);
        Ok(())
    }

    /// 解析函数，变量等的读取
    fn val(&mut self, istry: bool) -> AstError<()> {
        let t = self.next_token()?;
        if let Token::ID(token_data) = t {
            let idx = self.self_scope.borrow().get_sym(token_data);
            if idx.is_none() {
                self.try_err(
                    istry,
                    ErrorInfo::new(
                        t!(
                            SYMBOL_NOT_FOUND,
                            "0" =
                                self.token_lexer.borrow_mut().get_constpool().id_name[*token_data]
                        ),
                        t!(SYMBOL_ERROR),
                    ),
                )?
            }
            let idx = idx.unwrap();
            let nxt = self.next_token()?;
            match nxt {
                Token::LeftSmallBrace => {
                    let func_obj = self.self_scope.borrow().get_function(idx).unwrap();
                    let argv_list = self.opt_args(&func_obj)?;
                    // match ")"
                    self.get_token_checked(Token::RightSmallBrace)?;
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
                        self.add_bycode(
                            Opcode::CallNative,
                            Opidx(*(obj.buildin_id) as OpidxInternal),
                        );
                    } else if let Some(obj) = func_obj.downcast_ref::<CustomFunction>() {
                        self.add_bycode(Opcode::CallCustom, Opidx(*obj.custom_id as OpidxInternal));
                    }
                    // println!("{:?} {}", func_obj.get_io().return_type, self.cache.intty_id);
                    match func_obj.get_io().return_type {
                        None => {}
                        Some(v) => self.process_info.new_type(v),
                    }
                    return Ok(());
                }
                Token::DoubleColon => {
                    // 访问某个作用域里的东西
                    // println!("doublecolon");
                    let mut module = match self.self_scope.borrow().get_module(idx) {
                        Some(m) => m,
                        None => {
                            // println!("there!");
                            self.try_err(
                                istry,
                                ErrorInfo::new(
                                    t!(
                                        SYMBOL_NOT_FOUND,
                                        "0" = self.token_lexer.borrow_mut().get_constpool().id_name
                                            [*token_data]
                                    ),
                                    t!(SYMBOL_ERROR),
                                ),
                            )?
                        }
                    };
                    // println!("doublecolon");
                    swap(&mut self.self_scope, &mut module);
                    let ret = self.val(istry);
                    swap(&mut self.self_scope, &mut module);
                    return ret;
                }
                Token::LeftBigBrace => {
                    // 定义类
                    match self.self_scope.borrow().get_class(idx) {
                        None => {}
                        Some(_v) => return Ok(()),
                    }
                }
                Token::Dot => {
                    // 访问类成员
                }
                _ => {}
            }
            self.token_lexer.borrow_mut().next_back(nxt);
            self.load_var(idx, token_data, istry)?;
        } else {
            self.token_lexer.borrow_mut().next_back(t);
            self.try_err(
                istry,
                ErrorInfo::new(t!(UNEXPECTED_TOKEN, "0" = t), t!(SYNTAX_ERROR)),
            )?
        }
        Ok(())
    }

    fn item(&mut self, istry: bool) -> AstError<()> {
        let t = self.next_token()?;
        match t {
            Token::IntValue(data) => {
                self.add_bycode(Opcode::LoadInt, Opidx(*data as OpidxInternal));
                self.process_info.new_type(self.cache.intty_id);
            }
            Token::FloatValue(data) => {
                self.add_bycode(Opcode::LoadFloat, Opidx(*data as OpidxInternal));
                self.process_info.new_type(self.cache.floatty_id);
            }
            Token::StringValue(data) => {
                self.add_bycode(Opcode::LoadString, Opidx(*data as OpidxInternal));
                self.process_info.new_type(self.cache.strty_id);
            }
            Token::CharValue(data) => {
                self.add_bycode(Opcode::LoadChar, Opidx(*data as OpidxInternal));
                self.process_info.new_type(self.cache.charty_id);
            }
            Token::BoolValue(data) => {
                self.add_bycode(Opcode::LoadBool, Opidx(*data as OpidxInternal));
                self.process_info.new_type(self.cache.boolty_id);
            }
            _ => {
                self.token_lexer.borrow_mut().next_back(t);
                self.val(istry)?
            }
        }
        Ok(())
    }

    fn unary_opcode_impl(&mut self, istry: bool, optoken: Token) -> AstError<()> {
        let class_obj = self
            .self_scope
            .borrow()
            .get_class_by_class_id(self.process_info.pop_last_ty().unwrap())
            .unwrap();
        let oride = class_obj.get_override_func(optoken.convert_to_override().unwrap());
        match oride {
            Some(v) => {
                let tmp = v.io.check_argvs(vec![]);
                match tmp {
                    Ok(_) => {
                        self.add_bycode(v.opcode, NO_ARG);
                        self.process_info.new_type(v.io.return_type.unwrap())
                    }
                    Err(e) => {
                        let msg = self.gen_args_error(e);
                        self.try_err(istry, msg)?
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
        Ok(())
    }

    fn factor(&mut self, istry: bool) -> AstError<()> {
        let next_token = self.next_token()?;
        match next_token {
            Token::Sub => {
                self.factor(istry)?;
                self.unary_opcode_impl(istry, Token::SelfNegative)?;
            }
            Token::BitNot => {
                self.factor(istry)?;
                self.unary_opcode_impl(istry, Token::BitNot)?;
            }
            Token::Not => {
                self.factor(istry)?;
                self.unary_opcode_impl(istry, Token::Not)?;
            }
            Token::Add => {
                self.factor(istry)?;
            }
            Token::LeftSmallBrace => {
                self.expr(istry)?;
                self.get_token_checked(Token::RightSmallBrace)?;
            }
            Token::LeftMiddleBrace => {
                // 数组定义
                // 获取数组长度
                self.expr(istry)?;
                // 检查是否是整数
                match self.process_info.pop_last_ty() {
                    None => {
                        return self.gen_error(ErrorInfo::new(t!(EXPECTED_EXPR), t!(SYNTAX_ERROR)));
                    }
                    Some(ty) => {
                        if ty != self.cache.intty_id {
                            return self
                                .gen_error(ErrorInfo::new(t!(ARRAY_LEN_INT), t!(TYPE_ERROR)));
                        }
                    }
                }
                self.get_token_checked(Token::Semicolon)?;
                // 获取数组元素类型
                // TODO:在此处加上对于Default的检查
                let _t = self.lex_ty(istry)?;
            }
            _ => {
                self.token_lexer.borrow_mut().next_back(next_token);
                self.item(istry)?;
            }
        }
        Ok(())
    }

    fn lex_case(
        &mut self,
        expected_ty: ClassIdxId,
        mut add_expr_addr: impl FnMut(usize),
        mut add_case_final_addr: impl FnMut(usize),
    ) -> AstError<bool> {
        let mut is_end = false;
        // 储存要跳进case的指令的地址
        let mut jump_into_case = vec![];
        loop {
            // 处理通配符
            let t = self.next_token()?;
            if let Token::ID(data) = t
                && self.token_lexer.borrow_mut().get_constpool().id_name[*data] == "_"
            {
                is_end = true;
                self.add_bycode(Opcode::Jump, ARG_WRONG);
                jump_into_case.push(self.staticdata.get_last_opcode_id());
            } else {
                self.token_lexer.borrow_mut().next_back(t);
                self.factor(false)?;
                match self.process_info.pop_last_ty() {
                    None => {
                        return self.gen_error(ErrorInfo::new(t!(EXPECTED_EXPR), t!(SYNTAX_ERROR)));
                    }
                    Some(v) => {
                        if v != expected_ty {
                            return self
                                .gen_error(ErrorInfo::new(t!(TYPE_NOT_THE_SAME), t!(TYPE_ERROR)));
                        }
                        self.add_bycode(self.eq_without_pop(v), NO_ARG);
                    }
                }
                // 添加跳转指令
                self.add_bycode(Opcode::JumpIfTrue, ARG_WRONG);
                jump_into_case.push(self.staticdata.get_last_opcode_id());
            }
            let t = self.next_token()?;
            if t == Token::BitOr {
            } else if t == Token::Arrow {
                break;
            } else {
                self.gen_unexpected_token_error(t)?;
            }
        }
        // 这个是跳转到下一个case的
        self.add_bycode(Opcode::Jump, ARG_WRONG);
        add_expr_addr(self.staticdata.get_last_opcode_id().into());
        for i in &jump_into_case {
            let idx: usize = (*i).into();
            self.staticdata.inst[idx].operand.0 = self.staticdata.get_next_opcode_id();
        }
        self.get_token_checked(Token::LeftBigBrace)?;
        self.lex_until(RightBigBrace)?;
        // 最后还需要跳转指令
        self.add_bycode(Opcode::Jump, ARG_WRONG);
        add_case_final_addr(self.staticdata.get_last_opcode_id().into());
        Ok(is_end)
    }

    fn match_lex(&mut self) -> AstError<()> {
        self.expr(false)?;
        self.get_token_checked(Token::LeftBigBrace)?;
        match self.process_info.pop_last_ty() {
            None => {
                return self.gen_error(ErrorInfo::new(t!(EXPECTED_EXPR), t!(SYNTAX_ERROR)));
            }
            Some(v) => {
                let mut jump_condit_save: Vec<usize> = vec![];
                let mut jump_case_final = vec![];
                let mut end = false;
                loop {
                    for i in &jump_condit_save {
                        let t = self.staticdata.get_next_opcode_id();
                        self.staticdata.inst[*i].operand.0 = t;
                    }
                    jump_condit_save.clear();
                    let t = self.next_token()?;
                    if t == RightBigBrace {
                        break;
                    }
                    if end {
                        break;
                    }
                    self.token_lexer.borrow_mut().next_back(t);
                    end = self.lex_case(
                        v,
                        |line_num| jump_condit_save.push(line_num),
                        |line_num| jump_case_final.push(line_num),
                    )?;
                }
                for i in jump_case_final {
                    self.staticdata.inst[i].operand.0 = self.staticdata.get_next_opcode_id();
                }
            }
        }
        Ok(())
    }

    fn def_func(&mut self) -> AstError<()> {
        let funcname = self.get_token_checked_with_val(Token::ID(CONST_IDX_PLACEHOLDER))?;
        let name_id = self.insert_sym_with_error(funcname)?;
        // lex args
        self.get_token_checked(Token::LeftSmallBrace)?;
        let mut argname: ArgsNameTy = vec![];
        let mut ty_list: Vec<ClassIdxId> = vec![];
        loop {
            let t = self.next_token()?;
            if t == Token::RightSmallBrace {
                break;
            }
            if t != Token::Comma {
                self.token_lexer.borrow_mut().next_back(t);
            }
            let name_id = self.get_token_checked_with_val(Token::ID(CONST_IDX_PLACEHOLDER))?;
            argname.push(name_id);
            self.get_token_checked(Token::Colon)?;
            ty_list.push(self.lex_ty(false)?);
        }
        // 返回值解析
        let return_ty = self.lex_ty(true).ok();
        let io = IOType::new(ty_list, return_ty, false);
        self.get_token_checked(Token::LeftBigBrace)?;
        let mut function_body = vec![];
        let mut cnt = 1;
        loop {
            let t = self.next_token()?;
            function_body.push((t, self.compiler_data.borrow().context.get_line()));
            if t == RightBigBrace {
                cnt -= 1;
                if cnt == 0 {
                    break;
                }
            } else if t == Token::LeftBigBrace {
                cnt += 1;
            }
        }
        let function_id = self.module_manager.borrow_mut().alloc_custom_function_id();
        self.self_scope.borrow_mut().add_custom_function(
            name_id,
            function_id,
            CustomFunction::new(
                io,
                argname,
                self.token_lexer.borrow_mut().get_constpool().id_name[*funcname].clone(),
            ),
            function_body,
        );
        Ok(())
    }

    fn lex_class_item_loop(&mut self, class_obj: &mut CustomType) -> AstError<()> {
        loop {
            let t = self.next_token()?;
            if t == RightBigBrace {
                break;
            }
            self.token_lexer.borrow_mut().next_back(t);
            self.lex_class_item(class_obj)?;
        }
        Ok(())
    }

    fn lex_class_item(&mut self, class_obj: &mut CustomType) -> AstError<()> {
        let t = self.next_token()?;
        match t {
            Token::Var => {
                // 声明属性
                let attr_name_tok =
                    self.get_token_checked_with_val(Token::ID(CONST_IDX_PLACEHOLDER))?;
                let _attr_id = self.insert_sym_with_error(attr_name_tok)?;
                self.get_token_checked(Token::Colon)?;
                let ty = self.lex_ty(false)?;
                class_obj.add_attr(attr_name_tok, ty);
            }
            Token::Pub => {
                let mut is_in_pub = true;
                swap(&mut self.self_scope.borrow_mut().is_pub, &mut is_in_pub);
                self.get_token_checked(Token::LeftBigBrace)?;
                self.lex_class_item_loop(class_obj)?;
                self.get_token_checked(RightBigBrace)?;
                swap(&mut self.self_scope.borrow_mut().is_pub, &mut is_in_pub);
            }
            Token::Func => {
                self.def_func()?;
            }
            _ => {
                self.gen_error(ErrorInfo::new(
                    t!(UNEXPECTED_TOKEN, "0" = t),
                    t!(SYNTAX_ERROR),
                ))?;
            }
        }
        Ok(())
    }

    fn def_class(&mut self) -> AstError<()> {
        // new scope
        self.self_scope = Rc::new(RefCell::new(SymScope::new(SymScopePrev::Prev(
            self.self_scope.clone(),
        ))));
        self.self_scope.borrow_mut().in_class = true;
        let name = self.get_token_checked_with_val(Token::ID(CONST_IDX_PLACEHOLDER))?;
        let name_id = self.insert_sym_with_error(name)?;
        // FIXME:BUG!CLASS ID SHOULD BE ALLOCATED
        let mut class_obj = CustomType::new(
            ClassIdxId(*name_id),
            self.token_lexer.borrow_mut().get_constpool().id_name[*name].clone(),
        );
        self.get_token_checked(Token::LeftBigBrace)?;
        self.lex_class_item_loop(&mut class_obj)?;
        // 将作用域中剩下的函数加入作用域
        self.self_scope
            .borrow_mut()
            .add_type(name_id, Rc::new(class_obj))
            .unwrap();
        let prev_scope = self.self_scope.borrow().prev_scope.clone().unwrap();
        self.self_scope = prev_scope;
        Ok(())
    }

    fn lex_import_module(&mut self, istry: bool) -> AstError<()> {
        let tok = self.get_token_checked_with_val(Token::StringValue(CONST_IDX_PLACEHOLDER))?;
        // import的路径
        let mut path_with_dot = self.token_lexer.borrow_mut().get_constpool().id_str[*tok].clone();
        // 具体文件的路径
        let mut import_file_path = String::new();
        // 是不是dll
        let mut is_dll = false;
        if path_with_dot.starts_with("std") {
            // std特殊对待
            import_file_path = "std".to_string();
            is_dll = true;
        } else {
            // 判断是dll还是普通模块
            let tmp0 = self.compiler_data.clone();
            let tmp = tmp0.borrow();
            match tmp.option.inputsource.clone() {
                InputSource::File(now_module_path) => {
                    let path = PathBuf::from(path_with_dot.replace('.', "/"));
                    // 标识路径，但还没有做修改
                    let mut now_module_path = now_module_path;
                    now_module_path.pop();
                    now_module_path = now_module_path.join(path.clone());
                    let tmp = now_module_path.clone();
                    let file_name = match tmp.file_name() {
                        None => {
                            return self.try_err(istry, module_not_found(path.to_str().unwrap()));
                        }
                        Some(v) => v,
                    };
                    let file_name_dll = libloading::library_filename(file_name);
                    // 优先判断dll
                    now_module_path.set_file_name(file_name_dll);
                    if now_module_path.exists() {
                        // 是dll
                        is_dll = true;
                        now_module_path
                            .to_str()
                            .unwrap()
                            .clone_into(&mut import_file_path);
                        let tmp = self.module_manager.borrow().global_extern_function_id();
                        self.add_module(import_file_path.clone(), tmp)
                    } else {
                        // 不是dll，尝试判断trc文件
                        let file_name_trc = format!("{}{}", file_name.to_str().unwrap(), ".trc");
                        now_module_path.set_file_name(file_name_trc);
                        // println!("{}", now_module_path.display());
                        if now_module_path.exists() {

                            // 创建新的compiler来编译模块
                            // self.self_scope.as_any().borrow_mut().import_module();
                        } else {
                            return self.try_err(istry, module_not_found(path.to_str().unwrap()));
                        }
                    }
                }
                _ => {
                    return self.try_err(
                        istry,
                        ErrorInfo::new(t!(CANNOT_IMPORT_MODULE_WITHOUT_FILE), t!(SYMBOL_ERROR)),
                    );
                }
            }
        }
        if is_dll {
            // 导入对象可能是模块，也有可能是函数，类等，先单独截取出来
            let mut import_item_name = String::new();
            loop {
                // 从后往前截取出导入对象的名字
                let c = path_with_dot.pop().unwrap();
                if c == '.' {
                    break;
                }
                import_item_name = format!("{c}{import_item_name}");
            }
            let mut items = path_with_dot.split('.');
            let dll_lib = self
                .self_scope
                .borrow_mut()
                .get_dll(&import_file_path)
                .expect("error not found dll");
            let (lib_module, lib_storage) = load_module_storage(dll_lib.as_ref());
            // 删除模块名字
            items.next();
            let now = match lib_module.get_module(items) {
                Some(d) => d,
                None => {
                    return self.try_err(
                        istry,
                        ErrorInfo::new(t!(SYMBOL_NOT_FOUND, "0" = path_with_dot), t!(SYMBOL_ERROR)),
                    );
                }
            };
            match now.sub_modules().get(&import_item_name) {
                None => {
                    // 不是模块
                    match now.get_func_id_by_name(&import_item_name) {
                        None => {
                            // println!("here!{}", import_item_name);
                            return self.try_err(
                                istry,
                                ErrorInfo::new(
                                    t!(SYMBOL_NOT_FOUND, "0" = path_with_dot),
                                    t!(SYMBOL_ERROR),
                                ),
                            );
                        }
                        Some(func_item) => {
                            // println!("{}", import_item_name);
                            let func_item = now.functions()[func_item].1.clone();
                            // println!("{}", func_item.get_name());
                            let token_idx: ConstPoolData = self
                                .token_lexer
                                .borrow_mut()
                                .add_id(func_item.get_name().to_owned());
                            let func_extern_id = FuncIdx(
                                *func_item.buildin_id + self.modules_info[&import_file_path],
                            );
                            // println!("{}", func_extern_id);
                            let name = func_item.get_name().to_owned();
                            if let Err(e) = self.self_scope.borrow_mut().import_extern_func(
                                func_item,
                                token_idx,
                                &name,
                                func_extern_id,
                                lib_storage,
                                self.token_lexer.borrow_mut().get_constpool(),
                            ) {
                                return self.try_err(istry, e);
                            }
                        }
                    }
                }
                Some(module) => {
                    let tmp = self.token_lexer.borrow_mut().add_id(import_item_name);
                    let module_sym_idx: ScopeAllocId = self.insert_sym_with_error(tmp)?;
                    self.import_module_sym(module);
                    let sub_module = Rc::new(RefCell::new(SymScope::new(SymScopePrev::Prev(
                        self.self_scope.clone(),
                    ))));
                    self.self_scope
                        .borrow_mut()
                        .add_imported_module(module_sym_idx, sub_module.clone());
                    if let Err(e) = sub_module.borrow_mut().import_native_module(
                        module,
                        lib_storage,
                        self.token_lexer.borrow_mut().get_constpool(),
                        self.modules_info[&import_file_path],
                    ) {
                        return self.try_err(istry, e);
                    };
                }
            }
        } else {
            todo!()
        }
        Ok(())
    }

    /// 生成修改变量的指令
    fn modify_var(&mut self, varty: ClassIdxId, var_addr: usize, is_global: bool) {
        let objsz = self.get_ty_sz(varty);
        if !is_global {
            self.add_double_bycode(
                Opcode::StoreLocal,
                Opidx(var_addr as OpidxInternal),
                Opidx(objsz as OpidxInternal),
            );
        } else {
            self.add_double_bycode(
                Opcode::StoreGlobal,
                Opidx(var_addr as OpidxInternal),
                Opidx(objsz as OpidxInternal),
            );
        }
    }

    /// 生成新建变量的指令
    fn new_var(&mut self, name: ConstPoolData, varty: ClassIdxId) -> AstError<()> {
        let sym_idx = self.insert_sym_with_error(name)?;
        let (_var_sym, var_addr) =
            self.self_scope
                .borrow_mut()
                .add_var(sym_idx, varty, self.get_ty_sz(varty));
        self.modify_var(varty, var_addr, self.process_info.is_global);
        self.staticdata
            .update_var_table_mem_sz(self.self_scope.borrow().get_var_table_sz());
        Ok(())
    }

    fn store_var(&mut self, name: ConstPoolData) -> RuntimeResult<()> {
        self.expr(false)?;
        let var_type = match self.process_info.pop_last_ty() {
            Some(v) => v,
            None => {
                return self.gen_error(ErrorInfo::new(t!(EXPECTED_EXPR), t!(SYNTAX_ERROR)));
            }
        };
        self.new_var(name, var_type)?;
        Ok(())
    }

    fn assign_var(&mut self, name: ScopeAllocId) -> RuntimeResult<()> {
        self.expr(false)?;
        let var_type = match self.process_info.pop_last_ty() {
            Some(v) => v,
            None => {
                return self.gen_error(ErrorInfo::new(t!(EXPECTED_EXPR), t!(SYNTAX_ERROR)));
            }
        };
        let var = match self.self_scope.borrow().get_var(name) {
            Some(v) => v,
            None => {
                return self.gen_error(ErrorInfo::new(
                    t!(
                        SYMBOL_NOT_FOUND,
                        "0" = self.token_lexer.borrow_mut().get_constpool().id_name[*name]
                    ),
                    t!(SYMBOL_ERROR),
                ));
            }
        };
        if var.ty != var_type {
            return self.gen_error(ErrorInfo::new(t!(TYPE_NOT_THE_SAME), t!(TYPE_ERROR)));
        }
        self.modify_var(var.ty, var.addr, self.process_info.is_global);
        Ok(())
    }

    fn lex_until(&mut self, end_state: Token) -> AstError<()> {
        loop {
            let t = self.next_token()?;
            if t == end_state {
                break;
            }
            self.token_lexer.borrow_mut().next_back(t);
            self.statement()?;
        }
        Ok(())
    }

    fn lex_condit(&mut self) -> RuntimeResult<()> {
        self.expr(false)?;
        match self.process_info.pop_last_ty() {
            None => {
                return self.gen_error(ErrorInfo::new(t!(EXPECTED_EXPR), t!(SYNTAX_ERROR)));
            }
            Some(ty) => {
                if ty != self.cache.boolty_id {
                    return self.gen_error(ErrorInfo::new(t!(JUST_ACCEPT_BOOL), t!(TYPE_ERROR)));
                }
            }
        }
        Ok(())
    }

    fn if_lex(&mut self) -> RuntimeResult<()> {
        self.lex_condit()?;
        self.get_token_checked(Token::LeftBigBrace)?;
        // 每个分支末尾的跳转地址，用于跳出if
        let mut save_jump_opcode_idx = vec![];
        loop {
            let op_idx = self.staticdata.inst.len();
            // 本行是为了若条件不成立，跳转到下一个分支
            self.add_bycode(Opcode::JumpIfFalse, ARG_WRONG);
            self.lex_until(RightBigBrace)?;
            self.add_bycode(Opcode::Jump, ARG_WRONG);
            save_jump_opcode_idx.push(self.staticdata.get_last_opcode_id());
            self.staticdata.inst[op_idx].operand.0 = self.staticdata.get_next_opcode_id();
            let t = self.next_token()?;
            if t == Token::Else {
                let nxt_tok = self.next_token()?;
                if nxt_tok == Token::If {
                    self.lex_condit()?;
                    self.get_token_checked(Token::LeftBigBrace)?;
                    continue;
                }
                self.token_lexer.borrow_mut().next_back(nxt_tok);
                self.get_token_checked(Token::LeftBigBrace)?;
                self.lex_until(RightBigBrace)?;
                break;
            }
            // 小优化，最后一个分支不需要跳转出去
            save_jump_opcode_idx.pop();
            self.del_opcode().unwrap();
            *self.staticdata.inst[op_idx].operand.0 -= 1;
            self.token_lexer.borrow_mut().next_back(t);
            break;
        }
        for i in save_jump_opcode_idx {
            let idx: usize = i.into();
            self.staticdata.inst[idx].operand.0 = self.staticdata.get_next_opcode_id();
        }
        Ok(())
    }

    fn statement(&mut self) -> RuntimeResult<()> {
        let t = self.next_token()?;
        match t {
            Token::Continue => {
                if !self.self_scope.borrow().in_loop {
                    return self.gen_error(ErrorInfo::new(
                        t!(SHOULD_IN_LOOP, "0" = "continue"),
                        t!(SYNTAX_ERROR),
                    ));
                }
                self.add_bycode(Opcode::Jump, ARG_WRONG);
                self.self_scope
                    .borrow_mut()
                    .for_continue
                    .push(self.staticdata.get_last_opcode_id().into());
                return Ok(());
            }
            Token::Break => {
                if !self.self_scope.borrow().in_loop {
                    return self.gen_error(ErrorInfo::new(
                        t!(SHOULD_IN_LOOP, "0" = "break"),
                        t!(SYNTAX_ERROR),
                    ));
                }
                self.add_bycode(Opcode::Jump, ARG_WRONG);
                self.self_scope
                    .borrow_mut()
                    .for_break
                    .push(self.staticdata.get_last_opcode_id().into());
                return Ok(());
            }
            Token::Return => {
                if self.process_info.is_global {
                    return self.gen_error(ErrorInfo::new(
                        t!(RETURN_SHOULD_IN_FUNCTION),
                        t!(SYNTAX_ERROR),
                    ));
                }
                // ignore the result
                // for return and return expr both
                let ret_type = self.self_scope.borrow().func_io.unwrap();
                match ret_type {
                    Some(ty) => {
                        self.expr(true)?;
                        let actual_ty = self.process_info.pop_last_ty().unwrap();
                        if ty != actual_ty {
                            let s1 = self.get_ty_name_by_class_id(ty);
                            let s2 = self.get_ty_name_by_class_id(actual_ty);
                            return self.gen_error(ErrorInfo::new(
                                t!(RETURN_TYPE_ERROR, "0" = s1, "1" = s2),
                                t!(TYPE_ERROR),
                            ));
                        }
                    }
                    None => {
                        if self.expr(true).is_ok() {
                            let actual_ty = self.process_info.pop_last_ty().unwrap();
                            let name = self.get_ty_name_by_class_id(actual_ty);
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
            Token::Func => {
                self.def_func()?;
                return Ok(());
            }
            Token::Class => {
                self.def_class()?;
                return Ok(());
            }
            Token::While => {
                self.while_lex()?;
                return Ok(());
            }
            Token::For => {
                self.for_lex()?;
                return Ok(());
            }
            Token::If => {
                self.if_lex()?;
                return Ok(());
            }
            Token::Import => {
                self.lex_import_module(false)?;
                return Ok(());
            }
            Token::Match => {
                self.match_lex()?;
                return Ok(());
            }
            Token::ID(name) => {
                let tt = self.next_token()?;
                match tt {
                    Token::Assign => {
                        let var_id = match self.self_scope.borrow().get_sym(name) {
                            Some(v) => v,
                            None => {
                                return self.gen_error(ErrorInfo::new(
                                    t!(
                                        SYMBOL_NOT_FOUND,
                                        "0" = self.token_lexer.borrow_mut().get_constpool().id_name
                                            [*name]
                                    ),
                                    t!(SYMBOL_ERROR),
                                ));
                            }
                        };
                        self.assign_var(var_id)?;
                        return Ok(());
                    }
                    Token::Store => {
                        self.store_var(name)?;
                        return Ok(());
                    }
                    Token::SelfAdd => {}
                    Token::SelfSub => {}
                    Token::SelfMul => {}
                    Token::SelfDiv => {}
                    Token::SelfMod => {}
                    Token::SelfOr => {}
                    Token::SelfAnd => {}
                    _ => {
                        self.token_lexer.borrow_mut().next_back(tt);
                    }
                }
            }
            _ => {}
        }
        self.token_lexer.borrow_mut().next_back(t);
        self.expr(false)?;
        self.process_info.clear();
        Ok(())
    }

    /// # Return
    /// 返回函数的首地址
    fn lex_function(
        &mut self,
        funcid: ScopeAllocId,
        body: &FuncBodyTy,
    ) -> RuntimeResult<(usize, usize)> {
        if !self.first_func {
            // 如果不是第一个函数，在末尾加上结束主程序的指令
            // println!("run here.Inst num is {}", self.staticdata.inst.len());
            self.first_func = true;
            self.add_bycode(Opcode::Stop, NO_ARG);
            // println!(
            //     "run here.Inst num is {}.Op {}",
            //     self.staticdata.inst.len(),
            //     self.staticdata.get_next_opcode_id()
            // );
            self.staticdata.function_split = Some(self.staticdata.get_last_opcode_id());
        }
        let begin_inst_idx = self.staticdata.get_next_opcode_id();
        // println!("start addr:{}", begin_inst_idx);
        let func_obj = self
            .self_scope
            .borrow()
            .get_function(funcid)
            .unwrap()
            .downcast_rc::<CustomFunction>()
            .expect("Expect Custom Function");
        let io = func_obj.get_io();
        let tmp = self.self_scope.clone();
        // 解析参数
        self.self_scope = Rc::new(RefCell::new(SymScope::new(SymScopePrev::Prev(tmp.clone()))));
        self.self_scope.borrow_mut().func_io = Some(io.return_type);
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
            self.token_lexer.borrow_mut().next_back_with_line(i.0, i.1);
        }
        loop {
            let t = self.next_token()?;
            if t == RightBigBrace {
                break;
            }
            self.token_lexer.borrow_mut().next_back(t);
            self.statement()?;
        }
        if !self.staticdata.inst.is_empty()
            && self.staticdata.inst.last().unwrap().opcode != Opcode::PopFrame
        {
            self.add_bycode(Opcode::PopFrame, NO_ARG);
        }
        self.generate_func_in_scope()?;
        let mem_sz = self.self_scope.borrow().get_var_table_sz();
        self.self_scope = tmp.clone();
        Ok((begin_inst_idx.into(), mem_sz))
    }

    fn generate_func_in_scope(&mut self) -> AstError<()> {
        let mut tmp = vec![];
        swap(&mut tmp, &mut self.self_scope.borrow_mut().funcs_temp_store);
        for i in tmp {
            let (code_begin, var_mem_sz) = self.lex_function(i.0, &i.1)?;
            // println!("{}", code_begin);
            self.staticdata
                .funcs_pos
                .push(FuncStorage::new(code_begin, var_mem_sz))
        }
        Ok(())
    }

    pub fn generate_code(&mut self) -> RuntimeResult<()> {
        self.process_info.is_global = true;
        self.lex_until(Token::EndOfFile)?;
        // 结束一个作用域的代码解析后再解析这里面的函数
        self.process_info.is_global = false;
        self.generate_func_in_scope()?;
        Ok(())
    }

    pub fn modules_dll(&self) -> &[String] {
        &self.modules_dll
    }
}

#[cfg(test)]
mod tests {}
