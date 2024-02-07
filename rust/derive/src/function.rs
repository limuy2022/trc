use core::panic;

use quote::ToTokens;
use syn::{
    parse_str, punctuated, token::Comma, FnArg, PatType, ReturnType, Signature, Stmt, Type,
    TypePath,
};

pub fn process_function_def(sig: &mut Signature) -> (Vec<Stmt>, Vec<TypePath>, Type) {
    let output = sig.output.clone();
    let output: Type = match output {
        ReturnType::Default => parse_str("TypeAllowNull::No").unwrap(),
        ReturnType::Type(_, ty) => {
            if let Type::Path(name) = (*ty).clone() {
                let tyname = name.path.segments[0].ident.to_string();
                if tyname == "void" {
                    parse_str("TypeAllowNull::No").unwrap()
                } else if tyname == "any" {
                    parse_str("ANY_TYPE").unwrap()
                } else {
                    (*ty).clone()
                }
            } else {
                (*ty).clone()
            }
        }
    };
    let input_args = sig.inputs.clone();
    let mut new_stmts = vec![];
    let mut new_args: punctuated::Punctuated<FnArg, Comma> = punctuated::Punctuated::new();
    // 第一个参数是self
    if !input_args.is_empty() {
        if let FnArg::Receiver(_) = &input_args[0] {
            panic!("don't use self, use fn(DynaData) instead.")
        }
    }
    let mut args_type_required = vec![];

    new_args.push(parse_str::<FnArg>("dydata: &mut DynaData").unwrap());
    sig.inputs = new_args;
    for i in &input_args {
        if let FnArg::Typed(PatType { pat, ty, .. }, ..) = i {
            if let Type::Path(path, ..) = &**ty {
                let arg_name = match &**pat {
                    syn::Pat::Ident(syn::PatIdent { ident, .. }) => ident.to_string(),
                    _ => unreachable!(),
                };
                // println!("argv:{:#?}", path);
                if path.path.segments[0].ident == "any" {
                    args_type_required.push(parse_str("RustClass").unwrap());
                    new_stmts.push(
                        parse_str::<Stmt>(&format!(
                            "let mut {} = dydata.obj_stack.pop().unwrap();",
                            arg_name
                        ))
                        .unwrap(),
                    );
                } else {
                    args_type_required.push(path.clone());
                    new_stmts.push(
                        parse_str::<Stmt>(&format!(
                            "let mut {} = dydata.obj_stack.pop().unwrap().downcast::<{}>().unwrap();",
                            arg_name, path.to_token_stream()
                        ))
                        .unwrap(),
                    );
                }
            }
        }
    }
    // 栈是反的
    new_stmts.reverse();
    (new_stmts, args_type_required, output)
}

pub fn convent_to_info_func(name: impl Into<String>) -> String {
    format!("{}_info", name.into())
}