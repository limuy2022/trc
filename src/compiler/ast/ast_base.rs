use super::AstBuilder;
use super::AstError;
use crate::compiler::token::TokenType;
use crate::compiler::{scope::FuncIdxTy, token::Token};
use crate::compiler::{scope::SymScope, token::ConstPoolIndexTy};
use libcore::*;
use rust_i18n::t;
use std::{cell::RefCell, rc::Rc};

impl<'a> AstBuilder<'a> {
    pub fn clear_inst(&mut self) {
        self.staticdata.inst.clear();
        self.staticdata.function_split = None;
        self.first_func = false
    }

    #[inline]
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

    pub fn get_type_id(&self, ty_name: &str) -> Option<usize> {
        self.self_scope
            .borrow()
            .get_type_id_by_token(self.token_lexer.const_pool.name_pool[ty_name])
    }

    pub fn get_ty(&mut self, istry: bool) -> AstError<TyIdxTy> {
        let t = self.get_token_checked(TokenType::ID)?;
        let ty = match self
            .self_scope
            .borrow()
            .get_type_id_by_token(t.data.unwrap())
        {
            None => self.try_err(
                istry,
                ErrorInfo::new(
                    t!(
                        SYMBOL_NOT_FOUND,
                        "0" = self.token_lexer.const_pool.id_name[t.data.unwrap()]
                    ),
                    t!(TYPE_ERROR),
                ),
            )?,
            Some(v) => v,
        };
        Ok(ty)
    }

    pub fn gen_error<T>(&self, e: ErrorInfo) -> AstError<T> {
        self.token_lexer.compiler_data.report_compiler_error(e)
    }

    pub fn gen_unexpected_token_token<T>(&mut self, unexpcted: TokenType) -> AstError<T> {
        self.gen_error(ErrorInfo::new(
            t!(UNEXPECTED_TOKEN, "0" = unexpcted),
            t!(SYNTAX_ERROR),
        ))
    }

    pub fn get_token_checked(&mut self, ty: TokenType) -> AstError<Token> {
        let t = self.token_lexer.next_token()?;
        if t.tp != ty {
            self.token_lexer.next_back(t.clone());
            self.gen_unexpected_token_token(t.tp)?;
        }
        Ok(t)
    }

    pub fn del_opcode(&mut self) -> Result<(), &'static str> {
        match self.staticdata.inst.pop() {
            Some(_) => Ok(()),
            None => Err("stack empty"),
        }
    }

    fn gen_line_table(&mut self) {
        if !self.token_lexer.compiler_data.option.optimize {
            // 不生成行号表了
            self.staticdata
                .line_table
                .push(self.token_lexer.compiler_data.context.get_line())
        }
    }

    pub fn add_bycode(&mut self, opty: Opcode, opnum: usize) {
        self.staticdata.inst.push(Inst::new_single(opty, opnum));
        self.gen_line_table();
    }

    pub fn add_double_bycode(&mut self, opty: Opcode, opnum1: usize, opnum2: usize) {
        self.staticdata
            .inst
            .push(Inst::new_double(opty, opnum1, opnum2));
        self.gen_line_table();
    }

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

    pub fn get_ty_name(&mut self, type_name: TyIdxTy) -> String {
        self.self_scope
            .borrow()
            .get_class(type_name)
            .unwrap()
            .get_name()
            .to_string()
    }

    pub fn import_module_sym(&mut self, lib: &Module) {
        for i in lib.functions() {
            self.token_lexer.const_pool.add_id(i.0.clone());
        }
        for i in lib.classes() {
            self.token_lexer.const_pool.add_id(i.0.clone());
        }
    }

    pub fn insert_sym_with_error(&mut self, name: ConstPoolIndexTy) -> AstError<ScopeAllocIdTy> {
        match self.self_scope.borrow_mut().insert_sym(name) {
            Some(v) => Ok(v),
            None => self.gen_error(ErrorInfo::new(
                t!(
                    SYMBOL_REDEFINED,
                    "0" = self.token_lexer.const_pool.id_name[name]
                ),
                t!(SYMBOL_ERROR),
            ))?,
        }
    }

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

    pub fn get_scope(&self) -> Rc<RefCell<SymScope>> {
        self.self_scope.clone()
    }

    pub fn add_module(&mut self, module_name: String) {
        if self.modules_dll_dup.insert(module_name.clone()) {
            self.modules_dll.push(module_name);
        }
    }

    pub fn alloc_extern_function_id(&mut self) -> FuncIdxTy {
        self.module_manager.borrow_mut().alloc_extern_function_id()
    }
}
