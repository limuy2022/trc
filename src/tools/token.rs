use crate::compiler::{token::Token, ValuePool};
use logos::Logos;
use std::fs::read_to_string;

pub fn token(file_path: &str) {
    let s = read_to_string(file_path).unwrap();
    let lexer = Token::lexer_with_extras(&s, ValuePool::default());
    for token in lexer {
        println!("{}", token.unwrap());
    }
}
