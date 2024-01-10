use std::str::CharIndices;

use super::Compiler;
use std::io;

#[derive(PartialEq, Debug)]
enum TokenType {
    DOT,
    COMMA,
    INT_VALUE,
    STRING_VALUE,
    FLOAT_VALUE,
    LONG_INT_VALUE,
    LONG_FLOAT_VALUE,
}

#[derive(PartialEq, Debug)]
pub enum Data {
    Int(i32),
    Str(String),
    FLOAT(f64),
    NONEDATA,
}

#[derive(PartialEq, Debug)]
struct Token {
    tp: TokenType,
    data: Data,
}

struct TokenLex<'code, T: io::Read> {
    code: &'code str,
    pos: CharIndices<'code>,
    compiler_data: &'code mut Compiler<T>,
}

impl Token {
    fn new(tp: TokenType, data: Option<Data>) -> Token {
        match data {
            Some(data) => Token { tp, data },
            None => Token {
                tp,
                data: Data::NONEDATA,
            },
        }
    }
}

impl<T: io::Read> Iterator for TokenLex<'_, T> {
    type Item = Token;
    fn next(&mut self) -> Option<Self::Item> {
        self.next_token()
    }
}

impl<T: io::Read> TokenLex<'_, T> {
    fn new<'a>(code: &'a str, compiler_data: &'a mut Compiler<T>) -> TokenLex<'a, T> {
        TokenLex {
            code,
            pos: code.char_indices(),
            compiler_data,
        }
    }

    fn lex_symbol(&mut self, c: char) -> Token {
        match c {
            '.' => Token::new(TokenType::DOT, None),
            ',' => Token::new(TokenType::COMMA, None),
            _ => panic!("Not a symbol.Compiler error"),
        }
    }

    fn lex_num(&mut self, c: char) -> Token {
        // to save the int in str
        let mut s = String::new();
        // the radix of result
        let mut radix = 10;
        let presecnt_lex;
        if c == '0' {
            presecnt_lex = self.pos.next();
            match presecnt_lex {
                Some(c) => {
                    let c = c.1;
                    match c {
                        'x' | 'X' => {
                            s += "0x";
                            radix = 16;
                        }
                        'b' | 'B' => {
                            s += "0b";
                            radix = 2;
                        }
                        'o' | 'O' => {
                            s += "0o";
                            radix = 8;
                        }
                        _ => {}
                    }
                }
                None => {
                    return Token::new(TokenType::INT_VALUE, Some(Data::Int(0)));
                }
            }
        } else {
            s = c.to_string();
        }
        loop {
            match self.pos.next() {
                None => {
                    break;
                }
                Some(c) => {
                    let c = c.1;
                    if c.is_digit(radix) {
                        s.push(c);
                    } else {
                        self.pos.next_back();
                        break;
                    }
                }
            }
        }
        Token::new(
            TokenType::INT_VALUE,
            Some(Data::Int(s.parse().expect("wrong string to int"))),
        )
    }

    fn lex_str(&mut self) -> Token {
        let mut s = String::new();
        loop {}
    }

    fn next_token(&mut self) -> Option<Token> {
        let mut presecnt_lex = self.pos.next();
        loop {
            presecnt_lex = self.pos.next();
            match presecnt_lex {
                Some(c) => {
                    let c = c.1;
                    match c {
                        '\t' | ' ' => {
                            continue;
                        }
                        '\n' => {
                            self.compiler_data.line += 1;
                        }
                        _ => break,
                    }
                }
                None => {
                    return None;
                }
            }
        }
        let presecnt_lex = presecnt_lex.unwrap().1;
        if presecnt_lex.is_digit(10) {
            return Some(self.lex_num(presecnt_lex));
        }
        if presecnt_lex == '\'' || presecnt_lex == '"' {
            return Some(self.lex_str());
        }
        Some(self.lex_symbol(presecnt_lex))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn check<T: io::Read>(tokenlex: &mut TokenLex<T>, expected_res: Vec<Token>) {
        for i in expected_res {
            assert_eq!(i, tokenlex.next().unwrap());
        }
        assert_eq!(None, tokenlex.next());
    }

    #[test]
    fn test_numberlex() {
        let mut env = Compiler::new(io::stdin());
        let mut t = TokenLex::new(
            r#",,.,100


        123.9 232_304904
        0b011
        0x2aA4
        0o2434 0 0"#,
            &mut env,
        );
        let res = vec![
            Token::new(TokenType::COMMA, None),
            Token::new(TokenType::COMMA, None),
            Token::new(TokenType::DOT, None),
            Token::new(TokenType::COMMA, None),
            Token::new(TokenType::FLOAT_VALUE, Some(Data::FLOAT(123.9))),
            Token::new(TokenType::INT_VALUE, Some(Data::Int(232_304904))),
            Token::new(TokenType::INT_VALUE, Some(Data::Int(0b011))),
            Token::new(TokenType::INT_VALUE, Some(Data::Int(0x2aA4))),
            Token::new(TokenType::INT_VALUE, Some(Data::Int(0o2434))),
            Token::new(TokenType::INT_VALUE, Some(Data::Int(0))),
            Token::new(TokenType::INT_VALUE, Some(Data::Int(0))),
        ];
        check(&mut t, res);
    }

    #[test]
    fn test_symbol_lex() {}

    #[test]
    #[should_panic]
    fn test_wrong_number() {
        let mut env = Compiler::new(io::stdin());
        let t = TokenLex::new(r#"0xtghhy 0b231"#, &mut env);
        for _ in t {}
    }
}
