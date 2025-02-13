use core::panic;
use proc_macro::{TokenStream, TokenTree, token_stream::IntoIter};
use quote::quote;

use crate::function;

/// 解析[a=>b,c=>d]
pub fn lex_arrow(
    left_push: &mut Vec<syn::Ident>,
    right_push: &mut Vec<syn::Ident>,
    iter: &mut IntoIter,
    errormsg: &str,
) {
    match iter.next().unwrap() {
        TokenTree::Group(x, ..) => {
            let mut left_name = TokenStream::new();
            let mut right_name = TokenStream::new();
            let mut control_which_put = false;
            let mut iter = x.stream().into_iter();
            while let Some(i) = iter.next() {
                if let TokenTree::Punct(x) = i {
                    let x = x.to_string();
                    if x == "=" {
                        check_next_iter(&mut iter, ">");
                        // 切换左右边
                        control_which_put = true;
                    } else if x == "," {
                        // println!("item:{}", left_name);
                        left_push.push(syn::parse(left_name).expect("left push continue"));
                        right_push.push(syn::parse(right_name).expect("right push continue"));
                        left_name = TokenStream::new();
                        right_name = TokenStream::new();
                        control_which_put = false;
                    } else {
                        panic!("{}", errormsg);
                    }
                } else if !control_which_put {
                    left_name.extend(std::iter::once(i));
                } else {
                    right_name.extend(std::iter::once(i));
                }
            }
            if left_name.is_empty() && right_name.is_empty() {
                return;
            }
            left_push.push(
                syn::parse(left_name.clone())
                    .unwrap_or_else(|_| panic!("left push break, left name {}", left_name)),
            );
            right_push.push(syn::parse(right_name).expect("right push break"));
        }
        _ => {
            panic!("{}", errormsg);
        }
    }
}

pub fn check_next_iter(iter: &mut IntoIter, check_str: &str) {
    if let Some(TokenTree::Punct(x), ..) = iter.next() {
        if x.to_string() != check_str {
            panic!("expected {}", check_str);
        }
    } else {
        panic!("expected {}", check_str);
    }
}

/// lex [a,b,c]
///
/// # Panics
///
/// Panics if the structure is not like `[a,b,c]`.
fn lex_group(iter: &mut IntoIter, container: &mut Vec<syn::Ident>) {
    if let TokenTree::Group(x, ..) = iter.next().unwrap() {
        // println!("{}", x);
        let iter = x.stream().into_iter();
        for i in iter {
            if let TokenTree::Ident(x) = i {
                container.push(syn::parse_str::<syn::Ident>(&(x.to_string())).unwrap());
            } else if let TokenTree::Punct(x) = i {
                let x = x.to_string();
                if x != "," {
                    panic!(r#"expected ",".get {}"#, x);
                } else if x == "]" {
                    break;
                }
            }
        }
    } else {
        panic!("expected group")
    }
}

pub fn def_impl(context: TokenStream) -> TokenStream {
    let mut module_ident = None;
    let mut iter = context.into_iter();
    let mut left_func = vec![];
    let mut right_func = vec![];
    let mut left_class = vec![];
    let mut right_class = vec![];
    let mut submodules = vec![];
    let mut consts = vec![];
    while let Some(i) = iter.next() {
        match i {
            TokenTree::Ident(x) => {
                let x = x.to_string();
                if x == "module_name" {
                    check_next_iter(&mut iter, "=");
                    if let TokenTree::Ident(tmp) = iter.next().expect("name is expected") {
                        if module_ident.is_some() {
                            panic!("double defined");
                        }
                        module_ident =
                            Some(syn::parse_str::<syn::Ident>(&tmp.to_string()).unwrap());
                    } else {
                        panic!("name is expected");
                    }
                } else if x == "functions" {
                    check_next_iter(&mut iter, "=");
                    lex_arrow(
                        &mut left_func,
                        &mut right_func,
                        &mut iter,
                        "functions are expected",
                    )
                } else if x == "classes" {
                    check_next_iter(&mut iter, "=");
                    lex_arrow(
                        &mut left_class,
                        &mut right_class,
                        &mut iter,
                        "classes are expected",
                    );
                } else if x == "consts" {
                    check_next_iter(&mut iter, "=");
                    lex_group(&mut iter, &mut consts);
                } else if x == "submodules" {
                    check_next_iter(&mut iter, "=");
                    lex_group(&mut iter, &mut submodules);
                }
            }
            TokenTree::Punct(x) => {
                if x.to_string() != "," {
                    panic!("expected ,");
                }
            }
            _ => {
                panic!("grammar error unexpected {}", i.to_string());
            }
        }
    }
    for i in &mut left_func {
        *i = syn::parse_str::<syn::Ident>(&function::convert_to_info_func(i.to_string()))
            .expect("name error");
    }
    let ret = quote!(
        pub fn module_init(storage: &mut ModuleStorage) -> libcore::libbasic::Module {
            use libcore::libbasic::Module;
            use std::collections::hash_map::HashMap;
            let mut functions = vec![];
            let mut classes = HashMap::new();
            let mut submodules = HashMap::new();
            let mut consts_info = HashMap::new();
            #(
                classes.insert(stringify!(#right_class).to_string(), #left_class::init_info(Some(storage)));
            )*
            #(
                #left_class::gen_funcs_info(storage);
                #left_class::gen_overrides_info(storage);
                #left_class::modify_shadow_name(storage, stringify!(#right_class));
            )*
            #(
                consts_info.insert(stringify!(#consts).to_string(), #consts.to_string());
            )*
            #(
                functions.push((stringify!(#right_func).to_string(), #left_func(storage)));
            )*
            #(
                submodules.insert(stringify!(#submodules).to_string(), #submodules::module_init(storage));
            )*
            Module::new(
                stringify!(#module_ident),
                submodules,
                functions,
                classes,
                consts_info
            )
        }
    );
    ret.into()
}
