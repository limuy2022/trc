use function::process_function_def;
use proc_macro::TokenStream;
use quote::{quote, ToTokens};
use syn::ImplItem;
use syn::{
    parse_macro_input, parse_str, Expr, Ident, ItemFn, ItemImpl, ItemStruct, Stmt, Type, Visibility,
};

mod def_module;
mod function;

#[proc_macro_attribute]
/// 返回值一定要加上return
pub fn trc_function(attr: TokenStream, input: TokenStream) -> TokenStream {
    let mut input = parse_macro_input!(input as ItemFn);
    let (mut new_stmts, args_type_required, output) = process_function_def(&mut input.sig);
    let name = input.sig.ident.clone();
    input.sig.output = parse_str::<syn::ReturnType>("-> RuntimeResult<()>").expect("err1");
    let return_stmt = parse_str::<Stmt>("return Ok(());").expect("err2");
    for i in input.block.stmts {
        if let Stmt::Expr(Expr::Return(reexpr), ..) = i.clone() {
            if reexpr.expr.is_none() {
                continue;
            }
            let reexpr = *reexpr.expr.unwrap();
            let tmp = reexpr.to_token_stream().into_iter().next();
            if tmp.is_none() {
                continue;
            }
            let tmp = tmp.unwrap().to_string();

            if tmp == "Err" {
                new_stmts.push(i);
                continue;
            }
            new_stmts.push(
                parse_str::<Stmt>(&format!(
                    "dydata.obj_stack.push(Box::new({}));",
                    quote!(#reexpr)
                ))
                .unwrap(),
            );
            new_stmts.push(return_stmt.clone());
        } else {
            new_stmts.push(i);
        }
    }
    new_stmts.push(return_stmt.clone());
    input.block.stmts = new_stmts;
    // 将函数改写成正常的函数之后还需要生成一个函数来获取这个函数的信息，例如接收的参数和返回值
    let info_func_name =
        parse_str::<Ident>(&function::convent_to_info_func(name.to_string())).expect("name error");
    // println!("{}{:#?}", name.to_string(), info_function_self);
    let function_path: syn::Path;
    if let Some(ty) = attr.into_iter().next() {
        if ty.to_string() == "true" {
            function_path = parse_str(&format!("Self::{}", name)).unwrap();
        } else {
            function_path = parse_str(&name.to_string()).unwrap();
        }
    } else {
        function_path = parse_str(&name.to_string()).unwrap();
    }
    let rettmp = quote!(#input
        fn #info_func_name() -> RustFunction {
            use crate::base::stdlib::*;
            use crate::compiler::scope::TypeAllowNull;
            let ret_classes = vec![#(#args_type_required::export_info()),*];
            return RustFunction::new(stringify!(#name), #function_path, IOType::new(ret_classes, #output));
        }
    );
    // println!("{}", rettmp.to_token_stream());
    rettmp.into()
}

#[proc_macro]
/// def_module!(module_name = module_name, functions = [func1 => func1, func2 => func2], classes = [class1 => class1, class2 => class2], submodules=[submodule1, submodule2])
pub fn def_module(input: TokenStream) -> TokenStream {
    // let input = parse_macro_input!(input as DeriveInput);
    let processed = def_module::def_impl(input);
    processed
}

#[proc_macro_attribute]
pub fn trc_class(_: TokenStream, input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as ItemStruct);
    let name = input.ident.clone();
    // 为每个类生成一个export_info函数,控制导入到stdlib中的信息
    let mut members_ident: Vec<Ident> = vec![];
    let mut members_ty: Vec<Type> = vec![];
    for i in &input.fields {
        if let Visibility::Public(_) = i.vis {
            // 说明是要导出的成员
            let varname = i.ident.clone().unwrap();
            let vartype = i.ty.clone();
            if varname
                .to_token_stream()
                .into_iter()
                .next()
                .unwrap()
                .to_string()
                .starts_with('_')
            {
                continue;
            }
            members_ident.push(varname);
            members_ty.push(vartype);
        }
    }
    // export_info函数会调用method宏生成function_export函数
    // 目前的实现策略是先提供一个由once_cell储存的usize数，表示在类型表中的索引，里面储存该类型的Rc指针
    // 因为很可能某个函数的参数就是标准库中的某个类型，所以我们需要先将类型导入到class_table中
    let ret = quote!(#input
        use crate::base::stdlib::{RustClass, new_class_id, STD_CLASS_TABLE};
        use once_cell::sync::OnceCell;
        impl #name {
            pub fn init_info() -> usize {
                use std::collections::hash_map::HashMap;
                use crate::compiler::scope::Var;
                let mut members = HashMap::new();
                #(
                    members.insert(Var::new(stringify!(#members_ty), #members_ident));
                )*
                let classid = new_class_id();
                let mut ret = RustClass::new(
                    stringify!(#name),
                    members,
                    None,
                    Self::override_export(),
                    classid
                );
                STD_CLASS_TABLE.with(|std| {
                    std.borrow_mut().push(ret);
                });
                // let funcs_info = Self::function_export()
                // ret.functions = funcs_info;
                classid
            }

            pub fn gen_funcs_info() {
                STD_CLASS_TABLE.with(|std| {
                    std.borrow_mut()[Self::export_info()].functions = Self::function_export();
                });
            }

            pub fn export_info() -> usize {
                static ID: OnceCell<usize> = OnceCell::new();
                *ID.get_or_init(|| {
                    let id = Self::init_info();
                    id
                })
            }
        }
    );
    // println!("{}", ret.to_string());
    ret.into()
}

#[proc_macro_attribute]
/// 返回值一定要加上return
/// 定义一个函数体为空的函数会被当成接口声明，用于宏生成的接口
pub fn trc_method(_: TokenStream, input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as ItemImpl);
    // 生成一个function_export，返回一个vec,里面存着functions
    let name = input.self_ty.clone();
    // for i in input
    // print!("{:#?}", input);
    // println!("!!!!!!!!!!!!!!!!!!!!!!!!!!:{:#?}", name);
    let mut funcs = vec![];
    for i in &input.items {
        if let ImplItem::Fn(func) = i {
            if let Visibility::Public(_) = func.vis {
                funcs.push(
                    parse_str::<Ident>(&function::convent_to_info_func(func.sig.ident.to_string()))
                        .unwrap(),
                );
            }
        }
    }
    let ret = quote!(
    #input
    impl #name {
        fn function_export() -> HashMap<String, RustFunction> {
            let mut ret = HashMap::new();
            #(
                ret.insert(stringify!(#funcs).to_string(), Self::#funcs());
            )*
            ret
        }
    }
    );
    // println!("{}", ret);
    ret.into()
}
