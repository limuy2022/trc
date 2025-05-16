use quote::ToTokens;
use syn::{
    FnArg, PatType, ReturnType, Signature, Stmt, Type, TypePath, parse_str, punctuated,
    token::Comma,
};

pub fn process_function_def(sig: &mut Signature) -> (Vec<Stmt>, Vec<TypePath>, syn::Expr) {
    let output = sig.output.clone();
    let output: syn::Expr = match output {
        ReturnType::Default => parse_str("None").unwrap(),
        ReturnType::Type(_, ty) => {
            if let Type::Path(name) = (*ty).clone() {
                let tyname = name.path.segments[0].ident.to_string();
                if tyname == "void" {
                    parse_str("None").unwrap()
                } else if tyname == "any" {
                    parse_str("ANY_TYPE").unwrap()
                } else if tyname.ends_with("str") {
                    parse_str(r#"TypeAllowNull::Some(libcore::TrcStr::export_info())"#).unwrap()
                } else if tyname.ends_with("int") {
                    parse_str("TypeAllowNull::Some(libcore::TrcInt::export_info())").unwrap()
                } else if tyname.ends_with("bool") {
                    parse_str("TypeAllowNull::Some(libcore::TrcBool::export_info())").unwrap()
                } else if tyname.ends_with("char") {
                    parse_str("TypeAllowNull::Some(libcore::TrcChar::export_info())").unwrap()
                } else if tyname.ends_with("float") {
                    parse_str("TypeAllowNull::Some(libcore::TrcFloat::export_info())").unwrap()
                } else {
                    panic!("error");
                }
            } else {
                panic!("error");
            }
        }
    };
    let input_args = sig.inputs.clone();
    let mut new_stmts = vec![];
    let mut new_args: punctuated::Punctuated<FnArg, Comma> = punctuated::Punctuated::new();
    // 第一个参数是self
    if !input_args.is_empty() {
        if let FnArg::Receiver(_) = &input_args[0] {
            panic!("don't use self, use fn(DataTy) instead.")
        }
    }
    let mut args_type_required = vec![];

    new_args.push(parse_str::<FnArg>("dydata: &mut libcore::DynaData").unwrap());
    sig.inputs = new_args;
    for i in &input_args {
        if let FnArg::Typed(PatType { pat, ty, .. }, ..) = i {
            if let Type::Path(path, ..) = &**ty {
                let arg_name = match &**pat {
                    syn::Pat::Ident(syn::PatIdent { ident, .. }) => ident.to_string(),
                    _ => unreachable!(),
                };
                // println!("argv:{:#?}", path);
                let typename = path.path.to_token_stream().to_string();
                if typename.ends_with("any") {
                    args_type_required.push(parse_str("AnyType").unwrap());
                    new_stmts.push(
                        parse_str::<Stmt>(&format!(
                            r#"let mut {arg_name} = dydata.pop_data::<*mut dyn TrcObj>();"#
                        ))
                        .unwrap(),
                    );
                } else if typename.ends_with("str") {
                    args_type_required.push(parse_str("libcore::TrcStr").unwrap());
                    new_stmts.push(
                        parse_str(&format!(
                            r#"let mut {arg_name} = dydata.pop_data::<libcore::TrcStrInternal>();"#
                        ))
                        .unwrap(),
                    );
                } else if typename.ends_with("int") {
                    args_type_required.push(parse_str("libcore::TrcInt").unwrap());
                    new_stmts.push(
                        parse_str(&format!(
                            r#"let mut {arg_name} = dydata.pop_data::<libcore::TrcIntInternal>();"#
                        ))
                        .unwrap(),
                    );
                } else if typename.ends_with("bool") {
                    args_type_required.push(parse_str("libcore::TrcBool").unwrap());
                    new_stmts.push(
                        parse_str(&format!(
                            r#"let mut {arg_name} = dydata.pop_data::<bool>();"#
                        ))
                        .unwrap(),
                    );
                } else if typename.ends_with("char") {
                    args_type_required.push(parse_str("libcore::TrcChar").unwrap());
                    new_stmts.push(
                        parse_str(&format!(
                            r#"let mut {arg_name} = dydata.pop_data::<libcore::TrcCharInternal>();"#
                        ))
                        .unwrap(),
                    );
                } else if typename.ends_with("float") {
                    args_type_required.push(parse_str("libcore::TrcFloat").unwrap());
                    new_stmts.push(
                        parse_str(&format!(
                            r#"let mut {arg_name} = dydata.pop_data::<libcore::TrcFloatInternal>();"#
                        ))
                        .unwrap(),
                    );
                } else {
                    args_type_required.push(path.clone());
                    new_stmts.push(
                        parse_str::<Stmt>(&format!(
                            r#"let mut {} = unsafe{{
                                (&mut *dydata.pop_data::<*mut dyn TrcObj>()).downcast_mut::<{}>().expect("obj empty")
                            }};"#,
                            arg_name, path.to_token_stream()
                        ))
                        .expect("cast error"),
                    );
                }
            }
        }
    }
    // 栈是反的
    new_stmts.reverse();
    (new_stmts, args_type_required, output)
}

pub fn convert_to_info_func(name: impl Into<String>) -> String {
    format!("{}_info", name.into())
}
