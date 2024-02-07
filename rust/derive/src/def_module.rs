use core::panic;
use proc_macro::{token_stream::IntoIter, TokenStream, TokenTree};
use quote::quote;

use crate::function;

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
                // println!("{}", i);
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
            left_push.push(syn::parse(left_name).expect("left push break"));
            right_push.push(syn::parse(right_name).expect("right push break"));
        }
        _ => {
            panic!("{}", errormsg);
        }
    }
}

fn check_next_iter(iter: &mut IntoIter, check_str: &str) {
    if let Some(i) = iter.next() {
        if let TokenTree::Punct(x) = i {
            if x.to_string() != check_str {
                panic!("expected {}", check_str);
            }
        } else {
            panic!("expected {}", check_str);
        }
    } else {
        panic!("expected {}", check_str);
    }
}

pub fn def_impl(content: TokenStream) -> TokenStream {
    let mut module_ident = None;
    let mut iter = content.into_iter();
    let mut left_func = vec![];
    let mut right_func = vec![];
    let mut left_class = vec![];
    let mut right_class = vec![];
    let mut submodules = vec![];
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
                } else if x == "submodules" {
                    check_next_iter(&mut iter, "=");
                    if let TokenTree::Group(x, ..) = iter.next().unwrap() {
                        // println!("{}", x);
                        let iter = x.stream().into_iter();
                        for i in iter {
                            if let TokenTree::Ident(x) = i {
                                submodules
                                    .push(syn::parse_str::<syn::Ident>(&(x.to_string())).unwrap());
                            } else if let TokenTree::Punct(x) = i {
                                let x = x.to_string();
                                if x != "," {
                                    panic!("expected ,.get {}", x);
                                } else if x == "]" {
                                    break;
                                }
                            }
                        }
                    } else {
                        panic!("expected group")
                    }
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
        *i = syn::parse_str::<syn::Ident>(&function::convent_to_info_func(i.to_string()))
            .expect("name error");
    }
    let ret = quote!(
        pub fn init() -> Stdlib {
            use crate::base::stdlib::Stdlib;
            use std::collections::hash_map::HashMap;
            let mut functions = HashMap::new();
            let mut classes = HashMap::new();
            let mut submodules = HashMap::new();
            #(
                functions.insert(stringify!(#right_func).to_string(), #left_func());
            )*
            #(
                classes.insert(stringify!(#right_class).to_string(), #left_class::export_info());
                #left_class::gen_funcs_info();
                #left_class::gen_overrides_info();
                #left_class::modify_shadow_name(stringify!(#right_class));
            )*
            #(
                submodules.insert(stringify!(#submodules).to_string(), #submodules::init());
            )*
            Stdlib::new(
                stringify!(#module_ident),
                submodules,
                functions,
                classes
            )
        }
    );
    ret.into()
}
