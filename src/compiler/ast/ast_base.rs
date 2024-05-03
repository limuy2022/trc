use super::AstError;
use super::ModuleUnit;
use crate::compiler::token::Token;
use crate::compiler::{scope::SymScope, token::ConstPoolIndexTy};
use libcore::*;
use rust_i18n::t;
use std::{cell::RefCell, rc::Rc};

impl ModuleUnit {
    /// 清除之前编译留下的数据
    pub fn clear_inst(&mut self) {
        self.staticdata.inst.clear();
        self.staticdata.function_split = None;
        self.first_func = false
    }

    #[inline]
    /// 根据istry的值判断是否抛出错误
    pub fn try_err<T>(&self, istry: bool, info: ErrorInfo) -> AstError<T> {
        if istry {
            Err(LightFakeError::new().into())
        } else {
            self.gen_error(info)
        }
    }

    /// 获取对应类型在栈中的真实大小（内存对齐后）
    pub fn get_ty_sz(&self, id: TyIdxTy) -> usize {
        match self.convert_id_to_vm_ty(id) {
            VmStackType::Int => intsz!(),
            VmStackType::Float => floatsz!(),
            VmStackType::Str => strsz!(),
            VmStackType::Char => charsz!(),
            VmStackType::Bool => boolsz!(),
            VmStackType::Object => objsz!(),
        }
    }

    /// 将栈中的基本类型转换成Scope中的类型
    /// # Panic
    /// 如果不是基本类型，将会直接panic
    pub fn convert_vm_ty_to_id(&self, ty: VmStackType) -> TyIdxTy {
        match ty {
            VmStackType::Int => self.cache.intty_id,
            VmStackType::Float => self.cache.floatty_id,
            VmStackType::Str => self.cache.strty_id,
            VmStackType::Char => self.cache.charty_id,
            VmStackType::Bool => self.cache.boolty_id,
            VmStackType::Object => panic!("Error Type"),
        }
    }

    /// make sure code safe,by using match instead of if
    pub fn convert_id_to_vm_ty(&self, ty: TyIdxTy) -> VmStackType {
        if ty == self.cache.intty_id {
            return VmStackType::Int;
        }
        if ty == self.cache.floatty_id {
            return VmStackType::Float;
        }
        if ty == self.cache.strty_id {
            return VmStackType::Str;
        }
        if ty == self.cache.charty_id {
            return VmStackType::Char;
        }
        if ty == self.cache.boolty_id {
            return VmStackType::Bool;
        }
        VmStackType::Object
    }

    /// 该函数通过类型的名字得到对应的Scope中的id
    pub fn get_type_id(&self, ty_name: &str) -> Option<usize> {
        self.self_scope
            .borrow()
            .get_type_id_by_token(self.token_lexer.borrow_mut().const_pool.name_pool[ty_name])
    }

    /// 解析一个类型名为id
    pub fn lex_ty(&mut self, istry: bool) -> AstError<TyIdxTy> {
        let t = self.get_token_checked(Token::ID(0))?;
        let ty = match self.self_scope.borrow().get_type_id_by_token(t) {
            None => self.try_err(
                istry,
                ErrorInfo::new(
                    t!(
                        SYMBOL_NOT_FOUND,
                        "0" = self.token_lexer.borrow_mut().const_pool.id_name[t]
                    ),
                    t!(TYPE_ERROR),
                ),
            )?,
            Some(v) => v,
        };
        Ok(ty)
    }

    /// 根据错误信息，包装上上下文信息生成一个错误
    pub fn gen_error<T>(&self, e: ErrorInfo) -> AstError<T> {
        self.compiler_data.borrow().report_compiler_error(e)
    }

    /// token不正确，报出一个错误
    pub fn gen_unexpected_token_error<T>(&mut self, unexpcted: Token) -> AstError<T> {
        self.gen_error(ErrorInfo::new(
            t!(UNEXPECTED_TOKEN, "0" = unexpcted),
            t!(SYNTAX_ERROR),
        ))
    }

    /// 获取一个token并检查该token是否正确
    pub fn get_token_checked(&mut self, ty: Token) -> AstError<usize> {
        let t = self.token_lexer.borrow_mut().next_token()?;
        match (ty, t) {
            (Token::ID(_), Token::ID(data)) => Ok(data),
            _ => {
                self.token_lexer.borrow_mut().next_back(t);
                self.gen_unexpected_token_error(t)?
            }
        }
    }

    pub fn check_next_token(&mut self, ty: Token) -> AstError<()> {
        let t = self.token_lexer.borrow_mut().next_token()?;
        if t == ty {
            Ok(())
        } else {
            self.token_lexer.borrow_mut().next_back(t);
            self.gen_unexpected_token_error(t)
        }
    }

    /// 移除最后的一条指令
    pub fn del_opcode(&mut self) -> Result<(), &'static str> {
        // 删除指令
        if self.staticdata.inst.pop().is_none() {
            return Err("stack empty");
        };
        // 还有行号表
        if !self.compiler_data.borrow().option.optimize
            && self.staticdata.line_table.pop().is_none()
        {
            return Err("stack empty");
        };
        Ok(())
    }

    /// 生成一条指令的行号表
    fn gen_line_table(&mut self) {
        if !self.compiler_data.borrow().option.optimize {
            // 不生成行号表了
            self.staticdata
                .line_table
                .push(self.compiler_data.borrow().context.get_line())
        }
    }

    /// 添加一条单参数或无参数指令
    pub fn add_bycode(&mut self, opty: Opcode, opnum: Opidx) {
        self.staticdata.inst.push(Inst::new_single(opty, opnum));
        self.gen_line_table();
    }

    /// 添加一条双参数指令
    pub fn add_double_bycode(&mut self, opty: Opcode, opnum1: Opidx, opnum2: Opidx) {
        self.staticdata
            .inst
            .push(Inst::new_double(opty, opnum1, opnum2));
        self.gen_line_table();
    }

    /// 生成参数不匹配的错误信息
    pub fn gen_args_error(&mut self, info: ArguError) -> ErrorInfo {
        match info {
            ArguError::TypeNotMatch(ArgumentError { expected, actual }) => ErrorInfo::new(
                t!(
                    EXPECT_TYPE,
                    "0" = self.get_ty_name(expected),
                    "1" = self.get_ty_name(actual)
                ),
                t!(ARGUMENT_ERROR),
            ),
            ArguError::NumNotMatch(ArgumentError { expected, actual }) => ErrorInfo::new(
                t!(ARGU_NUMBER, "0" = expected, "1" = actual),
                t!(ARGUMENT_ERROR),
            ),
        }
    }

    /// 通过Scope中的ID获取类型名
    pub fn get_ty_name(&mut self, type_name: TyIdxTy) -> String {
        self.self_scope
            .borrow()
            .get_class(type_name)
            .unwrap()
            .get_name()
            .to_string()
    }

    /// 导入模块中的符号
    pub fn import_module_sym(&mut self, lib: &Module) {
        for i in lib.functions() {
            self.token_lexer.borrow_mut().const_pool.add_id(i.0.clone());
        }
        for i in lib.classes() {
            self.token_lexer.borrow_mut().const_pool.add_id(i.0.clone());
        }
    }

    /// 添加一个符号，在符号冲突的时候报出错误
    pub fn insert_sym_with_error(&mut self, name: ConstPoolIndexTy) -> AstError<ScopeAllocIdTy> {
        match self.self_scope.borrow_mut().insert_sym(name) {
            Some(v) => Ok(v),
            None => self.gen_error(ErrorInfo::new(
                t!(
                    SYMBOL_REDEFINED,
                    "0" = self.token_lexer.borrow_mut().const_pool.id_name[name]
                ),
                t!(SYMBOL_ERROR),
            ))?,
        }
    }

    // 生成EqWithoutPop指令
    pub fn eq_without_pop(&self, ty: ScopeAllocIdTy) -> Opcode {
        match self.convert_id_to_vm_ty(ty) {
            VmStackType::Int => Opcode::EqIntWithoutPop,
            VmStackType::Float => Opcode::EqFloatWithoutPop,
            VmStackType::Str => Opcode::EqStrWithoutPop,
            VmStackType::Char => Opcode::EqCharWithoutPop,
            VmStackType::Bool => Opcode::EqBoolWithoutPop,
            VmStackType::Object => Opcode::EqWithoutPop,
        }
    }

    /// 获取当前的Scope
    pub fn get_scope(&self) -> Rc<RefCell<SymScope>> {
        self.self_scope.clone()
    }

    /// 添加模块记录到数据中
    pub fn add_module(&mut self, module_name: String, function_base: usize) {
        if self.modules_dll_dup.insert(module_name.clone()) {
            self.modules_info.insert(module_name.clone(), function_base);
            self.modules_dll.push(module_name);
        }
    }

    /// 获取下一个Token,不检查
    pub fn next_token(&mut self) -> RuntimeResult<Token> {
        self.token_lexer.borrow_mut().next_token()
    }
}
