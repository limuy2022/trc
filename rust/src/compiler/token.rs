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
    // +=
    SELF_ADD,
    // -=
    SELF_SUB,
    // *=
    SELF_MUL,
    // /=
    SELF_DIV,
    // //=
    SELF_EXTRA_DIV,
    // %=
    SELF_MOD,
    // **
    POWER,
    // **=
    SELF_POWER,
    INT_VALUE,
    STRING_VALUE,
    FLOAT_VALUE,
    LONG_INT_VALUE,
    LONG_FLOAT_VALUE,
    // =
    ASSIGN,
    // :=
    STORE,
    // ==
    EQUAL,
    // !=
    UNEQUAL,
    // >
    GREATER,
    // <
    LESS,
    // <=
    LESS_EQUAL,
    // >=
    GREATER_EQUAL,
    // !
    NOT,
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

macro_rules! binary_symbol {
    ($a:expr, $b:expr, $binary_sym:expr, $sself:expr) => {{
        let c = $sself.compiler_data.input.read();
        if c == $binary_sym {
            return Token::new($b, None)
        }
        $sself.compiler_data.input.unread(c);
        Token::new($a, None)
    }}
}

macro_rules! self_symbol {
($sym:expr, $self_sym:expr, $sself:expr) =>
    {{
        binary_symbol!($sym, $self_sym, '=', $sself)
    }}
}

macro_rules! double_symbol {
    ($before_sym:expr, $before_self_sym:expr, $matched_sym:expr, $matched_self_sym:expr, matched_char:expr, $sself:expr) => {
        {
            let c = $sself.compiler_data.input.read();
            if c == $matched_char {
                return self_symbol!($matched_sym, $matched_self_sym, self)
            }
            self.compiler_data.input.unread(c);
            return self_symbol!($before_sym, $before_self_sym, self);
        }
    };
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
            '+' => self_symbol!(TokenType::ADD, TokenType::SELF_ADD, self),
            '-' => self_symbol!(TokenType::SUB, TokenType::SELF_SUB, self),
            '*' => {
                let c = self.compiler_data.input.read();
                if c == '*' {
                    return self_symbol!(TokenType::POWER, TokenType::SELF_POWER, self)
                }
                self.compiler_data.input.unread(c);
                return self_symbol!(TokenType::MUL, TokenType::SELF_MUL, self);
            },
            '%' => self_symbol!(TokenType::MOD, TokenType::SELF_MOD, self),
            '/' => {
                let c = self.compiler_data.input.read();
                if c == '=' {
                    return Token::new(TokenType::SELF_DIV, None)
                }
                self.compiler_data.input.unread(c);
                Token::new(TokenType::DIV, None)
            },
            '=' => binary_symbol!(TokenType::ASSIGN, TokenType::EQUAL, '=', self),
            '!' => binary_symbol!(TokenType::NOT, TokenType::UNEQUAL, '=', self),
            '>' => binary_symbol!(TokenType::GREATER, TokenType::GREATER_EQUAL, '=', self),
            '<' => binary_symbol!(TokenType::LESS, TokenType::LESS_EQUAL, '=', self),
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
                        self.compiler_data.content.add_line();
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
    fn test_symbol_lex() {
        let mut env = Compiler::new_string_compiler(
            Option::new(false, InputSource::StringInternal),
            r#":{}[]()+=%=//= // /=** *=*"#,
        );
        let mut t = TokenLex::new(&mut env);
        let res = vec![
            Token::new(TokenType::STRING_VALUE, Some(Data::Ind(0)))
        ];
        check(&mut t, res);
    }

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
