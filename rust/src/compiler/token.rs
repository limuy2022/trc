use super::{Compiler, Content, Float, INT_VAL_POOL_ZERO};
use crate::{
    base::error::{
        self, ErrorContent, ErrorInfo, RunResult, RuntimeError, FLOAT_OVER_FLOW, NUMBER_OVER_FLOW,
        PREFIX_FOR_FLOAT, SYNTAX_ERROR,
    },
    cfg::FLOAT_OVER_FLOW_LIMIT,
};
use gettextrs::gettext;
use lazy_static::lazy_static;
use std::{collections::HashMap, fmt::Display, process::exit};

#[derive(PartialEq, Debug, Clone)]
pub enum TokenType {
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
    SelfExactDiv,
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
    // :
    Colon,
    // ;
    Semicolon,
    ID,
    While,
    For,
    If,
    Else,
    Class,
    Match,
    Func,
    EndOfLine,
}

impl Display for TokenType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let res: String;
        match self {
            TokenType::Dot => res = ".".to_string(),
            TokenType::Comma => res = ",".to_string(),
            TokenType::LeftBigBrace => res = "{".to_string(),
            TokenType::RightBigBrace => res = "}".to_string(),
            TokenType::LeftMiddleBrace => res = "[".to_string(),
            TokenType::RightMiddleBrace => res = "]".to_string(),
            TokenType::LeftSmallBrace => res = "(".to_string(),
            TokenType::RightSmallBrace => res = ")".to_string(),
            TokenType::Add => res = "+".to_string(),
            TokenType::Sub => res = "-".to_string(),
            TokenType::Mul => res = "*".to_string(),
            TokenType::Div => res = "/".to_string(),
            TokenType::Mod => res = "%".to_string(),
            TokenType::ExactDiv => res = "//".to_string(),
            TokenType::BitNot => res = "~".to_string(),
            TokenType::BitLeftShift => res = "<<".to_string(),
            TokenType::BitRightShift => res = ">>".to_string(),
            TokenType::BitAnd => res = "&".to_string(),
            TokenType::BitOr => res = "|".to_string(),
            TokenType::Xor => res = "^".to_string(),
            TokenType::Power => res = "**".to_string(),
            TokenType::SelfAdd => res = "+=".to_string(),
            TokenType::SelfSub => res = "-=".to_string(),
            TokenType::SelfMul => res = "*=".to_string(),
            TokenType::SelfDiv => res = "/=".to_string(),
            TokenType::SelfExactDiv => res = "//=".to_string(),
            TokenType::SelfMod => res = "%=".to_string(),
            TokenType::SelfPower => res = "**=".to_string(),
            TokenType::SelfBitNot => res = "~=".to_string(),
            TokenType::SelfBitLeftShift => res = "<<=".to_string(),
            TokenType::SelfBitRightShift => res = ">>=".to_string(),
            TokenType::SelfBitAnd => res = "&=".to_string(),
            TokenType::SelfBitOr => res = "|=".to_string(),
            TokenType::SelfXor => res = "^=".to_string(),
            TokenType::IntValue => res = "integer".to_string(),
            TokenType::StringValue => res = "string".to_string(),
            TokenType::FloatValue => res = "float".to_string(),
            TokenType::LongIntValue => res = "long integer".to_string(),
            TokenType::Assign => res = "=".to_string(),
            TokenType::Store => res = ":=".to_string(),
            TokenType::Equal => res = "==".to_string(),
            TokenType::NotEqual => res = "!=".to_string(),
            TokenType::Greater => res = ">".to_string(),
            TokenType::Less => res = "<".to_string(),
            TokenType::LessEqual => res = "<=".to_string(),
            TokenType::GreaterEqual => res = ">=".to_string(),
            TokenType::Not => res = "!".to_string(),
            TokenType::Or => res = "||".to_string(),
            TokenType::And => res = "&&".to_string(),
            TokenType::Colon => res = ":".to_string(),
            TokenType::Semicolon => res = ";".to_string(),
            TokenType::ID => res = "identifier".to_string(),
            TokenType::While => res = "while".to_string(),
            TokenType::For => res = "for".to_string(),
            TokenType::If => res = "if".to_string(),
            TokenType::Else => res = "else".to_string(),
            TokenType::Class => res = "class".to_string(),
            TokenType::Match => res = "match".to_string(),
            TokenType::Func => res = "func".to_string(),
            TokenType::EndOfLine => res = "end of line".to_string(),
        }
        write!(f, "{}", res)
    }
}

#[derive(PartialEq, Debug)]
pub struct Token {
    pub tp: TokenType,
    pub data: Option<usize>,
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
    pub compiler_data: &'code mut Compiler,
    braces_check: Vec<BraceRecord>,
    unget_token: Vec<Token>,
}

impl Token {
    fn new(tp: TokenType, data: Option<usize>) -> Token {
        Token { tp, data }
    }
}

macro_rules! check_braces_match {
    ($sself:expr, $should_be_matched:expr, $brace_record:expr, $($front_brace:expr => $after_brace:expr),*) => {{
        match $brace_record.c {
            $(
                $front_brace => {
                    if $should_be_matched != $after_brace {
                        return Err(error::RuntimeError::new(
                            Box::new(Content::new_line(&$sself.compiler_data.content.module_name, $brace_record.line)),
                            ErrorInfo::new(
                                gettext!(error::UNMATCHED_BRACE, $brace_record.c),
                                gettext(error::SYNTAX_ERROR),
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

macro_rules! hash_map {
    ($($key:expr => $val:expr),*) => {
        {
            use std::collections::hash_map::HashMap;
            let mut ret = HashMap::new();
            $(
                ret.insert($key, $val);
            )*
            ret
        }
    };
}

lazy_static! {
    static ref KEYWORDS: HashMap<String, TokenType> = hash_map![
        String::from("while") => TokenType::While,
        String::from("for") => TokenType::For,
        String::from("if") => TokenType::If,
        String::from("else") => TokenType::Else,
        String::from("class") => TokenType::Class,
        String::from("func") => TokenType::Func,
        String::from("match") => TokenType::Match
    ];
}

enum NumValue {
    Integer(String),
    Float(String, String),
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
                    ErrorInfo::new(
                        gettext!(error::UNMATCHED_BRACE, c),
                        gettext(error::SYNTAX_ERROR),
                    ),
                ));
            }
            Some(cc) => {
                check_braces_match!(self, c, cc,
                    '{' => '}',
                    '[' => ']',
                    '(' => ')'
                );
                Ok(())
            }
        }
    }

    fn lex_id(&mut self, c: char) -> error::RunResult<Token> {
        Ok({
            let mut retname: String = String::from(c);
            loop {
                let c = self.compiler_data.input.read();
                if Self::is_id_char(c) {
                    retname.push(c);
                } else {
                    self.compiler_data.input.unread(c);
                    break;
                }
            }
            let tmp = KEYWORDS.get(&retname);
            match tmp {
                Some(val) => Token::new((*val).clone(), None),
                None => Token::new(
                    TokenType::ID,
                    Some(self.compiler_data.const_pool.add_id(retname)),
                ),
            }
        })
    }

    fn check_whether_symbol(c: char) -> bool {
        match c {
            '.' | ',' | '{' | '}' | '[' | ']' | '(' | ')' | '+' | '-' | '*' | '%' | '/' | '='
            | '!' | '>' | '<' | '~' | '^' | '|' | ':' | ';' => true,
            _ => false,
        }
    }

    fn is_useless_char(c: char) -> bool {
        match c {
            ' ' | '\n' | '\t' | '\0' => true,
            _ => false,
        }
    }

    fn is_string_begin(c: char) -> bool {
        match c {
            '"' | '\'' => true,
            _ => false,
        }
    }

    fn is_id_char(c: char) -> bool {
        if Self::check_whether_symbol(c)
            || c.is_digit(10)
            || Self::is_string_begin(c)
            || Self::is_useless_char(c)
        {
            false
        } else {
            true
        }
    }

    fn lex_symbol(&mut self, c: char) -> error::RunResult<Token> {
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
            '+' => self.self_symbol(TokenType::Add, TokenType::SelfAdd),
            '-' => self.self_symbol(TokenType::Sub, TokenType::SelfSub),
            '*' => self.double_symbol(
                TokenType::Mul,
                TokenType::SelfMul,
                TokenType::Power,
                TokenType::SelfPower,
                '*',
            ),
            '%' => self.self_symbol(TokenType::Mod, TokenType::SelfMod),
            '/' => self.double_symbol(
                TokenType::Div,
                TokenType::SelfDiv,
                TokenType::ExactDiv,
                TokenType::SelfExactDiv,
                '/',
            ),
            '=' => self.binary_symbol(TokenType::Assign, TokenType::Equal, '='),
            '!' => self.binary_symbol(TokenType::Not, TokenType::NotEqual, '='),
            '>' => self.double_symbol(
                TokenType::Greater,
                TokenType::GreaterEqual,
                TokenType::BitRightShift,
                TokenType::SelfBitRightShift,
                '>',
            ),
            '<' => self.double_symbol(
                TokenType::Less,
                TokenType::LessEqual,
                TokenType::BitLeftShift,
                TokenType::SelfBitLeftShift,
                '<',
            ),
            '~' => Token::new(TokenType::BitNot, None),
            '^' => Token::new(TokenType::Xor, None),
            '|' => self.binary_symbol(TokenType::Or, TokenType::BitOr, '|'),
            ':' => Token::new(TokenType::Colon, None),
            ';' => Token::new(TokenType::Semicolon, None),
            _ => {
                panic!("Not a symbol.Compiler error")
            }
        })
    }

    /// lex only an integer
    fn lex_num_integer(&mut self, c: char, radix: u32) -> String {
        let mut s = String::from(c);
        let mut presecnt_lex;
        loop {
            presecnt_lex = self.compiler_data.input.read();
            if presecnt_lex == '_' {
                continue;
            }
            if presecnt_lex.is_digit(radix) {
                s.push(presecnt_lex);
            } else {
                self.compiler_data.input.unread(presecnt_lex);
                break;
            }
        }
        s
    }

    fn lex_int_float(&mut self, mut c: char) -> RunResult<NumValue> {
        // the radix of result
        let mut radix = 10;
        let mut prefix = String::new();
        if c == '0' {
            // check the radix
            c = self.compiler_data.input.read();
            match c {
                'x' | 'X' => {
                    prefix = String::from("0x");
                    radix = 16;
                }
                'b' | 'B' => {
                    prefix = String::from("0b");
                    radix = 2;
                }
                'o' | 'O' => {
                    prefix = String::from("0o");
                    radix = 8;
                }
                _ => {
                    self.compiler_data.input.unread(c);
                    return Ok(NumValue::Integer(String::from("0")));
                }
            }
            c = self.compiler_data.input.read();
        }
        let intpart = format!("{prefix}{}", self.lex_num_integer(c, radix));
        if c == '.' {
            // float can be used with prefix
            if !prefix.is_empty() {
                return Err(RuntimeError::new(
                    Box::new(self.compiler_data.content.clone()),
                    ErrorInfo::new(gettext!(PREFIX_FOR_FLOAT, prefix), gettext(SYNTAX_ERROR)),
                ));
            }
            // float mode
            c = self.compiler_data.input.read();
            let float_part = self.lex_num_integer(c, radix);
            if float_part.len() + intpart.len() > FLOAT_OVER_FLOW_LIMIT {
                // overflow
                return Err(RuntimeError::new(
                    Box::new(self.compiler_data.content.clone()),
                    ErrorInfo::new(
                        gettext!(FLOAT_OVER_FLOW, format!("{intpart}.{float_part}")),
                        gettext(NUMBER_OVER_FLOW),
                    ),
                ));
            }
            return Ok(NumValue::Float(intpart, float_part));
        } else {
            self.compiler_data.input.unread(c);
        }
        Ok(NumValue::Integer(intpart))
    }

    fn turn_to_token(&mut self, val: NumValue) -> Token {
        match val {
            NumValue::Float(v1, v2) => Token::new(
                TokenType::FloatValue,
                Some(
                    self.compiler_data
                        .const_pool
                        .add_float(Float::new(v1.parse().unwrap(), v2.parse().unwrap())),
                ),
            ),
            NumValue::Integer(it) => Token::new(
                TokenType::IntValue,
                Some(self.compiler_data.const_pool.add_int(it.parse().unwrap())),
            ),
        }
    }

    fn lex_num(&mut self, mut c: char) -> RunResult<Token> {
        let tmp = self.lex_int_float(c)?;
        c = self.compiler_data.input.read();
        if c == 'e' || c == 'E' {
            c = self.compiler_data.input.read();
            let mut up: i32 = self.lex_num_integer(c, 10).parse().unwrap();
            match tmp {
                NumValue::Integer(mut it) => {
                    if up >= 0 {
                        // 保留int身份
                        for i in 0..up {
                            it.push('0');
                        }
                        return Ok(Token::new(
                            TokenType::IntValue,
                            Some(self.compiler_data.const_pool.add_int(it.parse().unwrap())),
                        ));
                    } else {
                        // 负数次，升级为float
                        let mut float_part = String::new();
                        up = -up;
                        for i in 0..up {
                            let tmp = it.pop();
                            match tmp {
                                None => {
                                    float_part.insert(0, '0');
                                }
                                Some(c) => {
                                    float_part.insert(0, c);
                                }
                            }
                        }
                        if it.is_empty() {
                            it = String::from("0");
                        }
                        return Ok(Token::new(
                            TokenType::FloatValue,
                            Some(self.compiler_data.const_pool.add_float(Float::new(
                                it.parse().unwrap(),
                                float_part.parse().unwrap(),
                            ))),
                        ));
                    }
                }
                NumValue::Float(mut v1, mut v2) => {
                    if up >= 0 {
                        for i in 0..up {
                            if v2.is_empty() {
                                v1.push('0');
                            } else {
                                let tmp = v2.remove(0);
                                v1.push(tmp);
                            }
                        }
                        if v2.is_empty() {
                            v2 = String::from("0");
                        }
                        return Ok(Token::new(
                            TokenType::FloatValue,
                            Some(
                                self.compiler_data.const_pool.add_float(Float::new(
                                    v1.parse().unwrap(),
                                    v2.parse().unwrap(),
                                )),
                            ),
                        ));
                    } else {
                        up = -up;
                        for i in 0..up {
                            let tmp = v1.pop();
                            match tmp {
                                Some(c) => {
                                    v2.insert(0, c);
                                }
                                None => {
                                    v2.insert(0, '0');
                                }
                            }
                        }
                        if v1.is_empty() {
                            v1 = String::from('0');
                        }
                        return Ok(Token::new(
                            TokenType::FloatValue,
                            Some(
                                self.compiler_data.const_pool.add_float(Float::new(
                                    v1.parse().unwrap(),
                                    v2.parse().unwrap(),
                                )),
                            ),
                        ));
                    }
                }
            }
        } else {
            self.compiler_data.input.unread(c);
            return Ok(self.turn_to_token(tmp));
        }
    }

    fn lex_str(&mut self, start_char: char) -> error::RunResult<Token> {
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
                    '0' => '\0',
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
                        gettext(error::SYNTAX_ERROR),
                    ),
                );
            }
        }
        Ok(Token::new(
            TokenType::StringValue,
            Some(self.compiler_data.const_pool.add_string(s)),
        ))
    }

    pub fn next_token(&mut self) -> error::RunResult<Option<Token>> {
        if !self.unget_token.is_empty() {
            let tmp = self.unget_token.pop().unwrap();
            if tmp.tp == TokenType::EndOfLine {
                self.compiler_data.content.add_line();
            }
            return Ok(Some(tmp));
        }
        let mut presecnt_lex;
        loop {
            presecnt_lex = self.compiler_data.input.read();
            match presecnt_lex {
                '\0' => {
                    return Ok(None);
                }
                '\t' | ' ' => {
                    continue;
                }
                '\n' => {
                    self.compiler_data.content.add_line();
                }
                _ => break,
            }
        }
        if presecnt_lex.is_digit(10) {
            return Ok(Some(self.lex_num(presecnt_lex)?));
        }
        if Self::is_string_begin(presecnt_lex) {
            return Ok(Some(self.lex_str(presecnt_lex)?));
        }
        if Self::check_whether_symbol(presecnt_lex) {
            return Ok(Some(self.lex_symbol(presecnt_lex)?));
        }
        Ok(Some(self.lex_id(presecnt_lex)?))
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
                    gettext(error::SYNTAX_ERROR),
                ),
            ));
        }
        Ok(())
    }

    fn binary_symbol(&mut self, a: TokenType, b: TokenType, binary_sym: char) -> Token {
        let c = self.compiler_data.input.read();
        if c == binary_sym {
            Token::new(b, None)
        } else {
            self.compiler_data.input.unread(c);
            Token::new(a, None)
        }
    }

    fn self_symbol(&mut self, sym: TokenType, self_sym: TokenType) -> Token {
        self.binary_symbol(sym, self_sym, '=')
    }

    fn double_symbol(
        &mut self,
        before_sym: TokenType,
        before_self_sym: TokenType,
        matched_sym: TokenType,
        matched_self_sym: TokenType,
        matched_char: char,
    ) -> Token {
        let c = self.compiler_data.input.read();
        if c == matched_char {
            self.self_symbol(matched_sym, matched_self_sym)
        } else {
            self.compiler_data.input.unread(c);
            self.self_symbol(before_sym, before_self_sym)
        }
    }
}

impl Drop for TokenLex<'_> {
    fn drop(&mut self) {
        // check the braces stack
        match self.check() {
            Err(e) => {
                eprintln!("{}", e);
                exit(1);
            }
            _ => {}
        }
    }
}

#[cfg(test)]
mod tests {
    use std::{collections::HashSet, hash::Hash};

    use super::*;
    use crate::compiler::{Float, InputSource, Option, Pool, INT_VAL_POOL_ONE};

    macro_rules! gen_test_token_env {
        ($test_string:expr, $env_name:ident) => {
            let mut env = Compiler::new_string_compiler(
                Option::new(false, InputSource::StringInternal),
                $test_string,
            );
            let mut $env_name = TokenLex::new(&mut env);
        };
    }

    fn check(tokenlex: &mut TokenLex, expected_res: Vec<Token>) {
        for i in expected_res {
            assert_eq!(i, tokenlex.next_token().unwrap().unwrap());
        }
        assert_eq!(None, tokenlex.next_token().unwrap());
        tokenlex.check().unwrap();
    }

    /// check const pool
    fn check_pool<T>(v: Vec<T>, pool_be_checked: &Pool<T>)
    where
        T: Eq + Hash + Clone,
    {
        let mut testpool: HashSet<T> = HashSet::new();
        for i in &v {
            testpool.insert((*i).clone());
        }
        assert_eq!(testpool.len(), pool_be_checked.len());
        for i in &testpool {
            assert!(pool_be_checked.contains_key(i));
        }
    }

    #[test]
    fn test_numberlex() {
        gen_test_token_env!(
            r#",,.,100
        
        
        123.9 232_304904
        0b011
        0x2aA4
        0o2434 0 0 1e3.8 1e9 1.2e1 8e-1"#,
            t
        );
        check(
            &mut t,
            vec![
                Token::new(TokenType::Comma, None),
                Token::new(TokenType::Comma, None),
                Token::new(TokenType::Dot, None),
                Token::new(TokenType::Comma, None),
                Token::new(TokenType::FloatValue, Some(0)),
                Token::new(TokenType::IntValue, Some(1)),
                Token::new(TokenType::IntValue, Some(2)),
                Token::new(TokenType::IntValue, Some(3)),
                Token::new(TokenType::IntValue, Some(4)),
                Token::new(TokenType::IntValue, Some(INT_VAL_POOL_ZERO)),
                Token::new(TokenType::IntValue, Some(INT_VAL_POOL_ZERO)),
                Token::new(TokenType::FloatValue, Some(1)),
                Token::new(TokenType::IntValue, Some(5)),
                Token::new(TokenType::FloatValue, Some(2)),
                Token::new(TokenType::FloatValue, Some(3)),
            ],
        );
        check_pool(
            vec![100, 232_304904, 0b011, 0x2aA4, 0, 1],
            &t.compiler_data.const_pool.const_ints,
        );
        check_pool(
            vec![
                Float::new(123, 9),
                Float::new(1, 2),
                Float::new(1000, 8),
                Float::new(0, 8),
            ],
            &t.compiler_data.const_pool.const_floats,
        );
    }

    #[test]
    fn test_symbol_lex() {
        gen_test_token_env!(
            r#":{}[]()+=%=//= // /=** *=*,
    >><< >>="#,
            t
        );
        check(
            &mut t,
            vec![
                Token::new(TokenType::Colon, None),
                Token::new(TokenType::LeftBigBrace, None),
                Token::new(TokenType::RightBigBrace, None),
                Token::new(TokenType::LeftMiddleBrace, None),
                Token::new(TokenType::RightMiddleBrace, None),
                Token::new(TokenType::LeftSmallBrace, None),
                Token::new(TokenType::RightSmallBrace, None),
                Token::new(TokenType::SelfAdd, None),
                Token::new(TokenType::SelfMod, None),
                Token::new(TokenType::SelfExactDiv, None),
                Token::new(TokenType::ExactDiv, None),
                Token::new(TokenType::SelfDiv, None),
                Token::new(TokenType::Power, None),
                Token::new(TokenType::SelfMul, None),
                Token::new(TokenType::Mul, None),
                Token::new(TokenType::Comma, None),
                Token::new(TokenType::BitRightShift, None),
                Token::new(TokenType::BitLeftShift, None),
                Token::new(TokenType::SelfBitRightShift, None),
            ],
        );
    }

    #[test]
    fn test_string_lex() {
        gen_test_token_env!(r#""s"'sd''sdscdcdfvf'"depkd"''"\n\t"'ttt\tt'"#, t);
        check(
            &mut t,
            vec![
                Token::new(TokenType::StringValue, Some(0)),
                Token::new(TokenType::StringValue, Some(1)),
                Token::new(TokenType::StringValue, Some(2)),
                Token::new(TokenType::StringValue, Some(3)),
                Token::new(TokenType::StringValue, Some(4)),
                Token::new(TokenType::StringValue, Some(5)),
                Token::new(TokenType::StringValue, Some(6)),
            ],
        );
        check_pool(
            vec![
                String::from("s"),
                String::from("sd"),
                String::from("sdscdcdfvf"),
                String::from("depkd"),
                String::from(""),
                String::from("\n\t"),
                String::from("ttt\tt"),
            ],
            &t.compiler_data.const_pool.const_strings,
        );
    }

    #[test]
    fn test_comprehensive_lex() {}

    #[test]
    fn test_id_lex() {
        gen_test_token_env!(r#"id fuck _fuck 天帝abc abc天帝"#, t);
        check(
            &mut t,
            vec![
                Token::new(TokenType::ID, Some(0)),
                Token::new(TokenType::ID, Some(1)),
                Token::new(TokenType::ID, Some(2)),
                Token::new(TokenType::ID, Some(3)),
                Token::new(TokenType::ID, Some(4)),
            ],
        );
        check_pool(
            vec![
                String::from("id"),
                String::from("fuck"),
                String::from("_fuck"),
                String::from("天帝abc"),
                String::from("abc天帝"),
            ],
            &t.compiler_data.const_pool.name_pool,
        );
    }

    #[test]
    fn test_wrong_number1() {
        gen_test_token_env!(r#"0b123"#, t);
        check(
            &mut t,
            vec![
                Token::new(TokenType::IntValue, Some(INT_VAL_POOL_ONE)),
                Token::new(TokenType::IntValue, Some(2)),
            ],
        );
        check_pool(vec![0b1, 23, 0], &t.compiler_data.const_pool.const_ints);
    }

    #[test]
    fn test_wrong_number2() {
        gen_test_token_env!(r#"0xabchds"#, t);
        check(
            &mut t,
            vec![
                Token::new(TokenType::IntValue, Some(2)),
                Token::new(TokenType::ID, Some(0)),
            ],
        );
        check_pool(vec![0xabc], &t.compiler_data.const_pool.const_ints);
        check_pool(
            vec![String::from("hds")],
            &t.compiler_data.const_pool.name_pool,
        );
    }

    #[test]
    fn test_next_back() {
        gen_test_token_env!(r#":()"#, t);
        let tmp = t.next_token().unwrap().unwrap();
        assert_eq!(tmp.tp, TokenType::Colon);
        t.next_back(tmp);
        assert_eq!(t.next_token().unwrap().unwrap().tp, TokenType::Colon);
        check(
            &mut t,
            vec![
                Token::new(TokenType::LeftSmallBrace, None),
                Token::new(TokenType::RightSmallBrace, None),
            ],
        );
    }

    #[test]
    #[should_panic]
    fn test_braces_check2() {
        gen_test_token_env!(r#":)|"#, t);
        check(
            &mut t,
            vec![
                Token::new(TokenType::Colon, None),
                Token::new(TokenType::LeftSmallBrace, None),
                Token::new(TokenType::BitAnd, None),
            ],
        );
    }

    #[test]
    #[should_panic]
    fn test_braces_check1() {
        gen_test_token_env!(r#":("#, t);
        check(
            &mut t,
            vec![
                Token::new(TokenType::Colon, None),
                Token::new(TokenType::LeftSmallBrace, None),
            ],
        );
    }
}
