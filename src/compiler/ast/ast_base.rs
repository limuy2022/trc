use super::AstBuilder;
use super::AstError;
use super::SymScope;
use crate::base::codegen::Inst;
use crate::base::codegen::Opcode;
use crate::base::codegen::VmStackType;
use crate::base::error::*;
use crate::compiler::scope::TyIdxTy;
use crate::compiler::token::Token;
use crate::compiler::token::TokenType;
use crate::compiler::ValuePool;
use crate::tvm::get_trcobj_sz;
use rust_i18n::t;
use std::cell::RefCell;
use std::mem::size_of;
use std::rc::Rc;

impl<'a> AstBuilder<'a> {
    pub fn clear_inst(&mut self) {
        self.staticdata.inst.clear();
        self.staticdata.function_split = None;
        self.first_func = false
    }

    pub fn report_error<T>(&self, info: ErrorInfo) -> AstError<T> {
        self.token_lexer.compiler_data.report_compiler_error(info)
    }

    #[inline]
    pub fn try_err<T>(&self, istry: bool, info: ErrorInfo) -> AstError<T> {
        if istry {
            Err(LightFakeError::new().into())
        } else {
            self.report_error(info)
        }
    }

    /// 获取对应类型的真实大小（内存对齐后）
    pub fn get_ty_sz(&self, id: TyIdxTy) -> usize {
        match self.convert_id_to_vm_ty(id) {
            VmStackType::Int => size_of::<i64>(),
            VmStackType::Float => size_of::<f64>(),
            VmStackType::Str => size_of::<*mut String>(),
            VmStackType::Char => size_of::<char>(),
            VmStackType::Bool => size_of::<bool>(),
            VmStackType::Object => get_trcobj_sz(),
        }
    }

    pub fn convert_vm_ty_to_id(&self, ty: VmStackType) -> TyIdxTy {
        match ty {
            VmStackType::Int => self.cache.intty_id,
            VmStackType::Float => self.cache.floatty_id,
            VmStackType::Str => self.cache.strty_id,
            VmStackType::Char => self.cache.charty_id,
            VmStackType::Bool => self.cache.boolty_id,
            VmStackType::Object => unreachable!(),
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

    pub fn get_type_id_internel(
        scope: Rc<RefCell<SymScope>>,
        const_pool: &ValuePool,
        ty_name: &str,
    ) -> Option<usize> {
        scope
            .as_ref()
            .borrow()
            .get_type(*const_pool.name_pool.get(ty_name).unwrap())
    }

    pub fn get_type_id(&self, ty_name: &str) -> Option<usize> {
        Self::get_type_id_internel(
            self.self_scope.clone(),
            &self.token_lexer.const_pool,
            ty_name,
        )
    }

    pub fn get_ty(&mut self, istry: bool) -> AstError<TyIdxTy> {
        let t = self.get_token_checked(TokenType::ID)?;
        let ty = match self.self_scope.as_ref().borrow().get_type(t.data.unwrap()) {
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
        self.try_err(false, e)
    }

    pub fn get_token_checked(&mut self, ty: TokenType) -> AstError<Token> {
        let t = self.token_lexer.next_token()?;
        if t.tp != ty {
            self.gen_error(ErrorInfo::new(
                t!(UNEXPECTED_TOKEN, "0" = t.tp),
                t!(SYNTAX_ERROR),
            ))?;
        }
        Ok(t)
    }

    pub fn del_opcode(&mut self) -> Result<(), ()> {
        match self.staticdata.inst.pop() {
            Some(_) => Ok(()),
            None => Err(()),
        }
    }

    pub fn add_bycode(&mut self, opty: Opcode, opnum: usize) {
        self.staticdata.inst.push(Inst::new(opty, opnum));
        if !self.token_lexer.compiler_data.option.optimize {
            // 不生成行号表了
            self.staticdata
                .line_table
                .push(self.token_lexer.compiler_data.context.get_line())
        }
    }
}
