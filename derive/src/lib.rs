use def_module::check_next_iter;
use function::process_function_def;
use proc_macro::{TokenStream, TokenTree};
use quote::{quote, ToTokens};
use syn::ImplItem;
use syn::{
    parse_macro_input, parse_str, Expr, Ident, ItemFn, ItemImpl, ItemStruct, Stmt, Type, Visibility,
};

mod def_module;
mod function;

#[proc_macro_attribute]
/// # warning
/// 返回值一定要加上return
/// # usage
/// 在开头加上#[macro@trc_function]即可，想表示任意类型就用any表示，想表示返回值为空就用void表示（留空当然可以）
/// 对可变参数的支持在于，你可以通过指定var_params=true来支持可变参数，可变参数会附加到你的最后一个参数后面，类型是任意，我们会把它读取到va_list Vec里面
/// 如果你要表示这是一个类的方法，你不能直接加上这个，还需要添加参数method=true，而你的方法需要写成类似fn a(type s, int a){}这样，第一个参数是你定义到的类型
/// 在这里面写基础类型，需要以trc内部的名字书写，例如，你不能写TrcInt,而需要写int,但是书写别的类型需要以rust内部定义的名字类书写，例如，写Sam而不是sam
pub fn trc_function(attr: TokenStream, input: TokenStream) -> TokenStream {
    let mut input = parse_macro_input!(input as ItemFn);
    let (mut new_stmts, args_type_required, output) = process_function_def(&mut input.sig);
    let name = input.sig.ident.clone();
    let mut function_path: syn::Path = parse_str(&name.to_string()).unwrap();
    let mut attr_iter = attr.into_iter();
    let mut if_enable_var_params: Expr = parse_str("false").unwrap();
    while let Some(i) = attr_iter.next() {
        match i {
            TokenTree::Ident(ident_name) => {
                let ident_name = ident_name.to_string();
                if ident_name == "method" {
                    check_next_iter(&mut attr_iter, "=");
                    let bol = attr_iter.next().unwrap().to_string();
                    if bol == "true" {
                        function_path = parse_str(&format!("Self::{}", name)).unwrap();
                    } else if bol == "false" {
                    } else {
                        panic!("expected true or false, not {}", bol);
                    }
                } else if ident_name == "var_params" {
                    check_next_iter(&mut attr_iter, "=");
                    let bol = attr_iter.next().unwrap().to_string();
                    if bol == "true" {
                        // 既然是可变参数，还要做一点手脚,可变参数是第一个到来的参数
                        if_enable_var_params = parse_str("true").unwrap();
                        let mut var_lex_tmp: Vec<Stmt> = vec![
                            parse_str("let mut va_list = vec![];").unwrap(),
                            parse_str(
                                "let args_num = dydata.pop_data::<libcore::TrcIntInternal>() as usize;",
                            )
                            .unwrap(),
                            parse_str("va_list.reserve(args_num);").unwrap(),
                            parse_str(
                                r#"for i in 0..args_num {
                            va_list.push(dydata.pop_data::<*mut dyn TrcObj>());
                        }"#,
                            )
                            .expect("Err on lex loop"),
                            parse_str("va_list.reverse();").unwrap(),
                        ];
                        // 将自己放到new_stmts的前面
                        var_lex_tmp.extend(new_stmts);
                        new_stmts = var_lex_tmp;
                    } else if bol == "false" {
                    } else {
                        panic!("expected true or false, not {}", bol);
                    }
                } else {
                    panic!("unexpected:{}", ident_name);
                }
            }
            TokenTree::Punct(sym) => {
                if sym.to_string() == "," {
                    continue;
                }
            }
            _ => {
                panic!("unexpected:{}", i);
            }
        }
    }
    input.sig.output =
        parse_str::<syn::ReturnType>("-> libcore::ErrorInfoResult<()>").expect("err1");
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
                parse_str::<Stmt>(&format!("dydata.push_data({});", quote!(#reexpr))).unwrap(),
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
        parse_str::<Ident>(&function::convert_to_info_func(name.to_string())).expect("name error");
    // println!("{}{:#?}", name.to_string(), info_function_self);

    let rettmp = quote!(#input
        fn #info_func_name(storage: &mut ModuleStorage) -> RustFunction {
            let ret_classes = vec![#(#args_type_required::export_info()),*];
            return RustFunction::new(stringify!(#name), #function_path, IOType::new(ret_classes, #output, #if_enable_var_params), storage);
        }
    );
    // println!("{}", rettmp.to_token_stream());
    rettmp.into()
}

#[proc_macro]
/// def_module!(module_name = module_name, functions = [func1 => func1, func2 => func2], classes = [class1 => class1, class2 => class2], submodules=[submodule1, submodule2])
pub fn def_module(input: TokenStream) -> TokenStream {
    def_module::def_impl(input)
}

#[proc_macro_attribute]
/// 用下划线开头的成员被认为不会导出
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
        impl #name {
            pub fn init_info(storage: Option<&mut ModuleStorage>) -> usize {
                use std::collections::hash_map::HashMap;
                static CLASS_ID: std::sync::OnceLock<usize> = std::sync::OnceLock::new();
                *CLASS_ID.get_or_init(|| {
                    let mut members = HashMap::new();
                    #(
                        members.insert(stringify!(#members_ty), stringify!(#members_ident));
                    )*
                    let tmp = RustClass::new_in_storage(
                        "",
                        members,
                        None,
                        None,
                        storage.expect("class info is called without initing")
                    );
                    //println!("init id");
                    tmp
                })
            }

            pub fn gen_funcs_info(storage: &mut ModuleStorage) {
                storage.access_class_mut(Self::export_info()).functions = Self::function_export(storage);
            }

            pub fn gen_overrides_info(storage: &mut ModuleStorage) {
                storage.access_class_mut(Self::export_info()).overrides = Self::override_export();
            }

            pub fn modify_shadow_name(storage: &mut ModuleStorage, name: &'static str) {
                storage.access_class_mut(Self::export_info()).name = name;
            }

            pub fn export_info() -> usize {
                Self::init_info(None)
            }
        }
    );
    // println!("{}", ret.to_string());
    ret.into()
}

#[proc_macro_attribute]
/// 返回值一定要加上return
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
                    parse_str::<Ident>(&function::convert_to_info_func(func.sig.ident.to_string()))
                        .unwrap(),
                );
            }
        }
    }
    let ret = quote!(
    #input
    impl #name {
        fn function_export(storage: &mut ModuleStorage) -> HashMap<String, RustFunction> {
            let mut ret = HashMap::new();
            #(
                ret.insert(stringify!(#funcs).to_string(), Self::#funcs(storage));
            )*
            ret
        }
    }
    );
    // println!("{}", ret);
    ret.into()
}

#[proc_macro_attribute]
pub fn trc_const(_: TokenStream, input: TokenStream) -> TokenStream {
    let mut input = parse_macro_input!(input as syn::ItemConst);
    // 将const a = c;改成 const a = stringify!(c);
    input.ty = parse_str("&str").unwrap();
    input.expr = parse_str(&format!("stringify!({})", input.expr.to_token_stream(),)).unwrap();
    quote!(#input).into()
}

#[proc_macro]
pub fn def_module_export(_t: TokenStream) -> TokenStream {
    quote!(
        fn import_lib() -> (&'static libcore::Module, &'static libcore::ModuleStorage) {
            // this is for any type
            static STDLIB_STORAGE: std::sync::OnceLock<ModuleStorage> = std::sync::OnceLock::new();
            static RETURN_VAL: std::sync::OnceLock<Module> = std::sync::OnceLock::new();
            let stro = STDLIB_STORAGE.get_or_init(|| {
                let mut storage = libcore::ModuleStorage::new();
                RETURN_VAL.get_or_init(|| module_init(&mut storage));
                storage
            });
            (RETURN_VAL.get().unwrap(), stro)
        }

        #[no_mangle]
        pub extern "C" fn get_lib() -> &'static libcore::Module {
            import_lib().0
        }

        #[no_mangle]
        pub extern "C" fn get_storage() -> &'static libcore::ModuleStorage {
            import_lib().1
        }
    )
    .into()
}
