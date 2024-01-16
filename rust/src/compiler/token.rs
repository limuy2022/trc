use gettextrs::gettext;

use crate::base::error;
use super::{Compiler, INT_VAL_POOL_ZERO};

#[derive(PartialEq, Debug)]
enum TokenType {
    // .
    DOT,
    // ,
    COMMA,
    // {
    LEFT_BIG_BRACE,
    // }
    RIGHT_BIG_BRACE,
    // [
    LEFT_MIDDLE_BRACE,
    // ]
    RIGHT_MIDDLE_BRACE,
    // (
    LEFT_SMALL_BRACE,
    // )
    RIGHT_SMALL_BRACE,
    // +
    ADD,
    // -
    SUB,
    // *
    MUL,
    // /
    DIV,
    // %
    MOD,
    // //
    EXACT_DIVISION,
    INT_VALUE,
    STRING_VALUE,
    FLOAT_VALUE,
    LONG_INT_VALUE,
    LONG_FLOAT_VALUE,
}

#[derive(PartialEq, Debug)]
pub enum Data {
    Ind(usize),
    NONEDATA,
}

#[derive(PartialEq, Debug)]
pub struct Token {
    tp: TokenType,
    data: Data,
}

pub struct TokenLex<'code> {
    compiler_data: &'code mut Compiler,
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

impl Iterator for TokenLex<'_> {
    type Item = Token;
    fn next(&mut self) -> Option<Self::Item> {
        self.next_token()
    }
}

impl TokenLex<'_> {
    pub fn new<'a>(compiler_data: &'a mut Compiler) -> TokenLex<'a> {
        TokenLex { compiler_data }
    }

    fn lex_symbol(&mut self, c: char) -> Token {
        match c {
            '.' => Token::new(TokenType::DOT, None),
            ',' => Token::new(TokenType::COMMA, None),
            '{' => Token::new(TokenType::LEFT_BIG_BRACE, None),
            '}' => Token::new(TokenType::RIGHT_BIG_BRACE, None),
            '[' => Token::new(TokenType::LEFT_MIDDLE_BRACE, None),
            ']' => Token::new(TokenType::RIGHT_MIDDLE_BRACE, None),
            '(' => Token::new(TokenType::LEFT_SMALL_BRACE, None),
            ')' => Token::new(TokenType::RIGHT_SMALL_BRACE, None),
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
            presecnt_lex = self.compiler_data.input.read();
            match presecnt_lex {
                '\0' => {
                    return Token::new(TokenType::INT_VALUE, Some(Data::Ind(INT_VAL_POOL_ZERO)));
                }
                _ => match presecnt_lex {
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
                },
            }
        } else {
            s = c.to_string();
        }
        loop {
            match self.compiler_data.input.read() {
                '\0' => {
                    break;
                }
                c => {
                    if c.is_digit(radix) {
                        s.push(c);
                    } else {
                        self.compiler_data.input.unread(c);
                        break;
                    }
                }
            }
        }
        Token::new(
            TokenType::INT_VALUE,
            Some(Data::Ind(self.compiler_data.const_pool.add_int(s.parse().expect("wrong string to int")))),
        )
    }

    fn lex_str(&mut self, start_char: char) -> Token {
        let mut s = String::new();
        let mut c = self.compiler_data.input.read();
        while c != start_char {
            if c == '\\' {
                c = self.compiler_data.input.read();
                c = match c {
                    't' => '\t',
                    'n' => '\n',
                    '\\' => '\\',
                    '"' => '"',
                    '\'' => '\'',
                    _ => {
                        s.push('\\');
                        c
                    }
                }
            }
            s.push(c);
            c = self.compiler_data.input.read();
            if c == '\0' {
                error::report_error(
                    &self.compiler_data.content,
                    error::ErrorInfo::new(
                        gettext!(error::STRING_WITHOUT_END, start_char),
                        error::SYNTAX_ERROR,
                    ),
                );
            }
        }
        Token::new(TokenType::STRING_VALUE, Some(Data::Ind(self.compiler_data.const_pool.add_string(s))))
    }

    fn next_token(&mut self) -> Option<Token> {
        let mut presecnt_lex = self.compiler_data.input.read();
        loop {
            match presecnt_lex {
                '\0' => {
                    return None;
                }
                c => match c {
                    '\t' | ' ' => {
                        continue;
                    }
                    '\n' => {
                        self.compiler_data.line += 1;
                    }
                    _ => break,
                },
            }
            presecnt_lex = self.compiler_data.input.read();
        }
        if presecnt_lex.is_digit(10) {
            return Some(self.lex_num(presecnt_lex));
        }
        if presecnt_lex == '\'' || presecnt_lex == '"' {
            return Some(self.lex_str(presecnt_lex));
        }
        Some(self.lex_symbol(presecnt_lex))
    }
}

#[cfg(test)]
mod tests {
    use crate::compiler::{InputSource, Option};

    use super::*;

    fn check(tokenlex: &mut TokenLex, expected_res: Vec<Token>) {
        for i in expected_res {
            assert_eq!(i, tokenlex.next().unwrap());
        }
        assert_eq!(None, tokenlex.next());
    }

    #[test]
    fn test_numberlex() {
        let mut env = Compiler::new_string_compiler(
            Option::new(false, InputSource::StringInternal),
            r#",,.,100


        123.9 232_304904
        0b011
        0x2aA4
        0o2434 0 0"#,
        );
        let mut t = TokenLex::new(&mut env);
        let res = vec![
            Token::new(TokenType::COMMA, None),
            Token::new(TokenType::COMMA, None),
            Token::new(TokenType::DOT, None),
            Token::new(TokenType::COMMA, None),
            Token::new(TokenType::FLOAT_VALUE, Some(Data::Ind(0))),
            Token::new(TokenType::INT_VALUE, Some(Data::Ind(1))),
            Token::new(TokenType::INT_VALUE, Some(Data::Ind(2))),
            Token::new(TokenType::INT_VALUE, Some(Data::Ind(3))),
            Token::new(TokenType::INT_VALUE, Some(Data::Ind(4))),
            Token::new(TokenType::INT_VALUE, Some(Data::Ind(INT_VAL_POOL_ZERO))),
            Token::new(TokenType::INT_VALUE, Some(Data::Ind(INT_VAL_POOL_ZERO))),
        ];
        check(&mut t, res);
    }

    #[test]
    fn test_symbol_lex() {}

    #[test]
    fn test_string_lex() {
        let mut env = Compiler::new_string_compiler(
            Option::new(false, InputSource::StringInternal),
            r#""s"'sd''sdscdcdfvf'"depkd"''"\n\t"'ttt\tt'"#,
        );
        let res = vec![
            Token::new(TokenType::STRING_VALUE, Some(Data::Ind(0)))
        ];
    }

    #[test]
    fn test_comprehensive_lex() {}

    #[test]
    #[should_panic]
    fn test_wrong_number() {
        let mut env = Compiler::new_string_compiler(
            Option::new(false, InputSource::StringInternal),
            r#"0xtghhy 0b231"#,
        );
        let t = TokenLex::new(&mut env);
        for _ in t {}
    }
}
