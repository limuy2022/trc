use super::{Compiler, Content, INT_VAL_POOL_ZERO};
use crate::base::error::{self, ErrorContent, ErrorInfo, RuntimeError};
use gettextrs::gettext;

#[derive(PartialEq, Debug)]
enum TokenType {
    // .
    Dot,
    // ,
    Comma,
    // {
    LeftBigBrace,
    // }
    RightBigBrace,
    // [
    LeftMiddleBrace,
    // ]
    RightMiddleBrace,
    // (
    LeftSmallBrace,
    // )
    RightSmallBrace,
    // +
    Add,
    // -
    Sub,
    // *
    Mul,
    // /
    Div,
    // %
    Mod,
    // //
    ExactDiv,
    // ~
    BitNot,
    // <<
    BitLeftShift,
    // >>
    BitRightShift,
    // &
    BitAnd,
    // |
    BitOr,
    // ^
    Xor,
    // **
    Power,
    // +=
    SelfAdd,
    // -=
    SelfSub,
    // *=
    SelfMul,
    // /=
    SelfDiv,
    // //=
    SelfExtraDiv,
    // %=
    SelfMod,
    // **=
    SelfPower,
    // ~=
    SelfBitNot,
    // <<=
    SelfBitLeftShift,
    // >>=
    SelfBitRightShift,
    // &=
    SelfBitAnd,
    // |=
    SelfBitOr,
    // ^=
    SelfXor,
    IntValue,
    StringValue,
    FloatValue,
    LongIntValue,
    LongFloatValue,
    // =
    Assign,
    // :=
    Store,
    // ==
    Equal,
    // !=
    NotEqual,
    // >
    Greater,
    // <
    Less,
    // <=
    LessEqual,
    // >=
    GreaterEqual,
    // !
    Not,
    // ||
    Or,
    // &&
    And,
    EndOfLine,
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

struct BraceRecord {
    c: char,
    line: usize,
}

impl BraceRecord {
    fn new(c: char, line: usize) -> BraceRecord {
        BraceRecord { c, line }
    }
}

pub struct TokenLex<'code> {
    compiler_data: &'code mut Compiler,
    braces_check: Vec<BraceRecord>,
    unget_token: Vec<Token>,
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

macro_rules! binary_symbol {
    ($a:expr, $b:expr, $binary_sym:expr, $sself:expr) => {{
        let c = $sself.compiler_data.input.read();
        if c == $binary_sym {
            Token::new($b, None)
        } else {
            $sself.compiler_data.input.unread(c);
            Token::new($a, None)
        }
    }};
}

macro_rules! self_symbol {
    ($sym:expr, $self_sym:expr, $sself:expr) => {{
        binary_symbol!($sym, $self_sym, '=', $sself)
    }};
}

macro_rules! double_symbol {
    ($before_sym:expr, $before_self_sym:expr, $matched_sym:expr, $matched_self_sym:expr, $matched_char:expr, $sself:expr) => {{
        let c = $sself.compiler_data.input.read();
        if c == $matched_char {
            self_symbol!($matched_sym, $matched_self_sym, $sself)
        } else {
            $sself.compiler_data.input.unread(c);
            self_symbol!($before_sym, $before_self_sym, $sself)
        }
    }};
}

macro_rules! check_braces_match {
    ($sself:expr, $brace_record:expr, $($front_brace:expr => $after_brace:expr),*) => {{
        match $brace_record.c {
            $(
                $front_brace => {
                    if $brace_record.c != $after_brace {
                        return Err(error::RuntimeError::new(
                            Box::new(Content::new_line(&$sself.compiler_data.content.module_name, $brace_record.line)),
                            ErrorInfo::new(
                                gettext!(error::UNMATCHED_BRACE, $brace_record.c),
                                error::SYNTAX_ERROR,
                            ),
                        ));
                    }
                },
            )*
            _ => {
                panic!("unmatched {}", $brace_record.c)
            }
        }
    }}
}

impl TokenLex<'_> {
    pub fn new<'a>(compiler_data: &'a mut Compiler) -> TokenLex<'a> {
        TokenLex {
            compiler_data,
            braces_check: vec![],
            unget_token: vec![],
        }
    }

    fn check_braces_stack(&mut self, c: char) -> Result<(), RuntimeError> {
        let top = self.braces_check.pop();
        match top {
            None => {
                return Err(RuntimeError::new(
                    Box::new(self.compiler_data.content.clone()),
                    ErrorInfo::new(gettext!(error::UNMATCHED_BRACE, c), error::SYNTAX_ERROR),
                ));
            }
            Some(c) => {
                check_braces_match!(self, c,
                    '{' => '}',
                    '[' => ']',
                    '(' => ')'
                );
                Ok(())
            }
        }
    }

    fn lex_symbol(&mut self, c: char) -> anyhow::Result<Token> {
        Ok(match c {
            '.' => Token::new(TokenType::Dot, None),
            ',' => Token::new(TokenType::Comma, None),
            '{' => {
                self.braces_check
                    .push(BraceRecord::new(c, self.compiler_data.content.get_line()));
                Token::new(TokenType::LeftBigBrace, None)
            }
            '}' => {
                self.check_braces_stack(c)?;
                Token::new(TokenType::RightBigBrace, None)
            }
            '[' => {
                self.braces_check
                    .push(BraceRecord::new(c, self.compiler_data.content.get_line()));
                Token::new(TokenType::LeftMiddleBrace, None)
            }
            ']' => {
                self.check_braces_stack(c)?;
                Token::new(TokenType::RightMiddleBrace, None)
            }
            '(' => {
                self.braces_check
                    .push(BraceRecord::new(c, self.compiler_data.content.get_line()));
                Token::new(TokenType::LeftSmallBrace, None)
            }
            ')' => {
                self.check_braces_stack(c)?;
                Token::new(TokenType::RightSmallBrace, None)
            }
            '+' => self_symbol!(TokenType::Add, TokenType::SelfAdd, self),
            '-' => self_symbol!(TokenType::Sub, TokenType::SelfSub, self),
            '*' => double_symbol!(
                TokenType::Mul,
                TokenType::SelfMul,
                TokenType::Power,
                TokenType::SelfPower,
                '*',
                self
            ),
            '%' => self_symbol!(TokenType::Mod, TokenType::SelfMod, self),
            '/' => double_symbol!(
                TokenType::Div,
                TokenType::SelfDiv,
                TokenType::ExactDiv,
                TokenType::SelfExtraDiv,
                '/',
                self
            ),
            '=' => binary_symbol!(TokenType::Assign, TokenType::Equal, '=', self),
            '!' => binary_symbol!(TokenType::Not, TokenType::NotEqual, '=', self),
            '>' => binary_symbol!(TokenType::Greater, TokenType::GreaterEqual, '=', self),
            '<' => binary_symbol!(TokenType::Less, TokenType::LessEqual, '=', self),
            '~' => Token::new(TokenType::BitNot, None),
            '^' => Token::new(TokenType::Xor, None),
            '|' => {
                binary_symbol!(TokenType::Or, TokenType::BitOr, '|', self)
            }
            _ => panic!("Not a symbol.Compiler error"),
        })
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
                    return Token::new(TokenType::IntValue, Some(Data::Ind(INT_VAL_POOL_ZERO)));
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
            TokenType::IntValue,
            Some(Data::Ind(
                self.compiler_data
                    .const_pool
                    .add_int(s.parse().expect("wrong string to int")),
            )),
        )
    }

    fn lex_str(&mut self, start_char: char) -> anyhow::Result<Token> {
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
                error::RuntimeError::new(
                    Box::new(self.compiler_data.content.clone()),
                    error::ErrorInfo::new(
                        gettext!(error::STRING_WITHOUT_END, start_char),
                        error::SYNTAX_ERROR,
                    ),
                );
            }
        }
        Ok(Token::new(
            TokenType::StringValue,
            Some(Data::Ind(self.compiler_data.const_pool.add_string(s))),
        ))
    }

    fn next_token(&mut self) -> anyhow::Result<Option<Token>> {
        if !self.unget_token.is_empty() {
            let tmp = self.unget_token.pop().unwrap();
            if tmp.tp == TokenType::EndOfLine {
                self.compiler_data.content.add_line();
            }
            return Ok(Some(tmp));
        }
        let mut presecnt_lex = self.compiler_data.input.read();
        loop {
            match presecnt_lex {
                '\0' => {
                    return Ok(None);
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
            return Ok(Some(self.lex_num(presecnt_lex)));
        }
        if presecnt_lex == '\'' || presecnt_lex == '"' {
            return Ok(Some(self.lex_str(presecnt_lex)?));
        }
        Ok(Some(self.lex_symbol(presecnt_lex)?))
    }

    fn next_back(&mut self, t: Token) {
        if t.tp == TokenType::EndOfLine {
            self.compiler_data.content.del_line();
        }
        self.unget_token.push(t);
    }

    pub fn check(&mut self) -> Result<(), RuntimeError> {
        if !self.braces_check.is_empty() {
            let unmatch_char = self.braces_check.pop().unwrap();
            return Err(error::RuntimeError::new(
                Box::new(Content::new_line(
                    &self.compiler_data.content.module_name,
                    unmatch_char.line,
                )),
                ErrorInfo::new(
                    gettext!(error::UNMATCHED_BRACE, unmatch_char.c),
                    error::SYNTAX_ERROR,
                ),
            ));
        }
        Ok(())
    }
}

impl Drop for TokenLex<'_> {
    fn drop(&mut self) {
        // check the braces stack
    }
}

#[cfg(test)]
mod tests {
    use crate::compiler::{InputSource, Option};

    use super::*;

    fn check(tokenlex: &mut TokenLex, expected_res: Vec<Token>) {
        for i in expected_res {
            assert_eq!(i, tokenlex.next_token().unwrap().unwrap());
        }
        assert_eq!(None, tokenlex.next_token().unwrap());
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
            Token::new(TokenType::Comma, None),
            Token::new(TokenType::Comma, None),
            Token::new(TokenType::Dot, None),
            Token::new(TokenType::Comma, None),
            Token::new(TokenType::FloatValue, Some(Data::Ind(0))),
            Token::new(TokenType::IntValue, Some(Data::Ind(1))),
            Token::new(TokenType::IntValue, Some(Data::Ind(2))),
            Token::new(TokenType::IntValue, Some(Data::Ind(3))),
            Token::new(TokenType::IntValue, Some(Data::Ind(4))),
            Token::new(TokenType::IntValue, Some(Data::Ind(INT_VAL_POOL_ZERO))),
            Token::new(TokenType::IntValue, Some(Data::Ind(INT_VAL_POOL_ZERO))),
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
        let res = vec![Token::new(TokenType::StringValue, Some(Data::Ind(0)))];
        check(&mut t, res);
    }
    #[test]
    fn test_string_lex() {
        let mut env = Compiler::new_string_compiler(
            Option::new(false, InputSource::StringInternal),
            r#""s"'sd''sdscdcdfvf'"depkd"''"\n\t"'ttt\tt'"#,
        );
        let res = vec![Token::new(TokenType::StringValue, Some(Data::Ind(0)))];
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
        let mut t = TokenLex::new(&mut env);
        t.next_token().unwrap();
        t.next_token().unwrap();
    }
}
