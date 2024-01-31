use super::{Compiler, Content, Float};
use crate::{
    base::error::{
        self, ErrorContent, ErrorInfo, RunResult, RuntimeError, FLOAT_OVER_FLOW, NUMBER_OVER_FLOW,
        PREFIX_FOR_FLOAT, SYNTAX_ERROR,
    },
    cfg::FLOAT_OVER_FLOW_LIMIT,
    hash_map,
};
use gettextrs::gettext;
use lazy_static::lazy_static;
use std::{collections::HashMap, fmt::Display, process::exit};

#[derive(PartialEq, Debug, Clone)]
pub enum TokenType {
    // ->
    Arrow,
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
    // ||=
    SelfOr,
    // &&=
    SelfAnd,
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
    Import,
    Return,
    EndOfLine,
    EndOfFile,
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
            TokenType::EndOfLine => res = "EOL".to_string(),
            TokenType::EndOfFile => res = "EOF".to_string(),
            TokenType::Import => res = "import".to_string(),
            TokenType::Arrow => res = "->".to_string(),
            TokenType::Return => res = "return".to_string(),
            TokenType::SelfAnd => res = "&&=".to_string(),
            TokenType::SelfOr => res = "||=".to_string(),
        }
        write!(f, "{}", res)
    }
}

#[derive(PartialEq, Debug, Clone)]
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

lazy_static! {
    static ref KEYWORDS: HashMap<String, TokenType> = hash_map![
        "while".to_string() => TokenType::While,
        "for".to_string() => TokenType::For,
        "if".to_string() => TokenType::If,
        "else".to_string() => TokenType::Else,
        "class".to_string() => TokenType::Class,
        "func".to_string() => TokenType::Func,
        "match".to_string() => TokenType::Match,
        "return".to_string() => TokenType::Return,
        "import".to_string() => TokenType::Import
    ];
    static ref RADIX_TO_PREFIX: HashMap<usize, &'static str> = hash_map![
        2 => "0b",
        8 => "0o",
        16 => "0x"
    ];
}

enum NumValue {
    Integer(i64),
    FloatVal(Float),
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
            '-' => {
                let c = self.compiler_data.input.read();
                if c == '>' {
                    Token::new(TokenType::Arrow, None)
                } else if c == '=' {
                    Token::new(TokenType::SelfSub, None)
                } else {
                    self.compiler_data.input.unread(c);
                    Token::new(TokenType::Sub, None)
                }
            }
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
            '^' => self.self_symbol(TokenType::Xor, TokenType::SelfXor),
            ':' => self.binary_symbol(TokenType::Colon, TokenType::Store, '='),
            ';' => Token::new(TokenType::Semicolon, None),
            '|' => self.double_symbol(
                TokenType::BitOr,
                TokenType::SelfBitOr,
                TokenType::Or,
                TokenType::SelfOr,
                '|',
            ),
            '&' => self.double_symbol(
                TokenType::BitAnd,
                TokenType::SelfBitAnd,
                TokenType::And,
                TokenType::SelfAnd,
                '&',
            ),
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
        if c == '0' {
            // check the radix
            c = self.compiler_data.input.read();
            match c {
                'x' | 'X' => {
                    radix = 16;
                }
                'b' | 'B' => {
                    radix = 2;
                }
                'o' | 'O' => {
                    radix = 8;
                }
                _ => {
                    self.compiler_data.input.unread(c);
                    return Ok(NumValue::Integer(0));
                }
            }
            c = self.compiler_data.input.read();
        }
        let intpart = format!("{}", self.lex_num_integer(c, radix));
        c = self.compiler_data.input.read();
        if c == '.' {
            // float can be used with prefix
            if radix != 10 {
                return Err(RuntimeError::new(
                    Box::new(self.compiler_data.content.clone()),
                    ErrorInfo::new(
                        gettext!(PREFIX_FOR_FLOAT, RADIX_TO_PREFIX[&(radix as usize)]),
                        gettext(SYNTAX_ERROR),
                    ),
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
            return Ok(NumValue::FloatVal(Float::new(
                i64::from_str_radix(&intpart, radix).unwrap(),
                i64::from_str_radix(&float_part, radix).unwrap(),
                Self::cal_zero(&float_part),
            )));
        } else {
            self.compiler_data.input.unread(c);
        }
        Ok(NumValue::Integer(
            i64::from_str_radix(&intpart, radix).unwrap(),
        ))
    }

    fn turn_to_token(&mut self, val: NumValue) -> Token {
        match val {
            NumValue::FloatVal(v) => Token::new(
                TokenType::FloatValue,
                Some(self.compiler_data.const_pool.add_float(v)),
            ),
            NumValue::Integer(it) => Token::new(
                TokenType::IntValue,
                Some(self.compiler_data.const_pool.add_int(it)),
            ),
        }
    }

    fn cal_zero(s: &str) -> usize {
        let mut zero = 0;
        for i in s.chars() {
            if i == '0' {
                zero += 1;
            }
        }
        return zero;
    }

    fn lex_num(&mut self, mut c: char) -> RunResult<Token> {
        let tmp = self.lex_int_float(c)?;
        c = self.compiler_data.input.read();
        if c == 'e' || c == 'E' {
            c = self.compiler_data.input.read();
            let mut up_flag: i64 = 1;
            if c == '+' {
                c = self.compiler_data.input.read();
            } else if c == '-' {
                up_flag = -1;
                c = self.compiler_data.input.read();
            }
            let mut up: i64 = self.lex_num_integer(c, 10).parse().unwrap();
            up *= up_flag;
            match tmp {
                NumValue::Integer(mut it) => {
                    if up >= 0 {
                        // 保留int身份
                        for _ in 0..up {
                            it *= 10;
                        }
                        return Ok(Token::new(
                            TokenType::IntValue,
                            Some(self.compiler_data.const_pool.add_int(it)),
                        ));
                    } else {
                        // 负数次，升级为float
                        let mut float_part = String::new();
                        up = -up;
                        for _ in 0..up {
                            if it == 0 {
                                float_part.insert(0, '0');
                            } else {
                                float_part = (it % 10).to_string() + &float_part;
                                it /= 10;
                            }
                        }
                        return Ok(Token::new(
                            TokenType::FloatValue,
                            Some(self.compiler_data.const_pool.add_float(Float::new(
                                it,
                                float_part.parse().unwrap(),
                                Self::cal_zero(&float_part),
                            ))),
                        ));
                    }
                }
                NumValue::FloatVal(mut v) => {
                    if up >= 0 {
                        let mut s = v.back.to_string();
                        for _ in 0..up {
                            if s.is_empty() {
                                v.front *= 10;
                            } else {
                                v.front *= 10;
                                v.front += s.remove(0) as i64 - '0' as i64;
                            }
                        }
                        if s.is_empty() {
                            v = Float::new(v.front, 0, 0);
                        } else {
                            v.zero = Self::cal_zero(&s);
                            v.back = s.parse().unwrap();
                        }
                        return Ok(Token::new(
                            TokenType::FloatValue,
                            Some(self.compiler_data.const_pool.add_float(v)),
                        ));
                    } else {
                        up = -up;
                        let mut s = String::new();
                        for _ in 0..up {
                            if v.front == 0 {
                                v.zero += 1;
                            } else {
                                s = (v.front % 10).to_string() + &s;
                                v.front /= 10;
                            }
                        }
                        s += &v.back.to_string();
                        v.zero += Self::cal_zero(&s);
                        v.back = s.parse().unwrap();
                        return Ok(Token::new(
                            TokenType::FloatValue,
                            Some(self.compiler_data.const_pool.add_float(v)),
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

    pub fn next_token(&mut self) -> error::RunResult<Token> {
        if !self.unget_token.is_empty() {
            let tmp = self.unget_token.pop().unwrap();
            if tmp.tp == TokenType::EndOfLine {
                self.compiler_data.content.add_line();
            }
            return Ok(tmp);
        }
        let mut presecnt_lex;
        loop {
            presecnt_lex = self.compiler_data.input.read();
            match presecnt_lex {
                '\0' => {
                    return Ok(Token::new(TokenType::EndOfFile, None));
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
            return Ok(self.lex_num(presecnt_lex)?);
        }
        if Self::is_string_begin(presecnt_lex) {
            return Ok(self.lex_str(presecnt_lex)?);
        }
        if Self::check_whether_symbol(presecnt_lex) {
            return Ok(self.lex_symbol(presecnt_lex)?);
        }
        Ok(self.lex_id(presecnt_lex)?)
    }

    pub fn next_back(&mut self, t: Token) {
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
    use std::{collections::HashSet, fmt::Debug, hash::Hash};

    use super::*;
    use crate::compiler::{Float, InputSource, Option, Pool, INT_VAL_POOL_ONE, INT_VAL_POOL_ZERO};

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
            assert_eq!(i, tokenlex.next_token().unwrap());
        }
        assert_eq!(TokenType::EndOfFile, tokenlex.next_token().unwrap().tp);
        tokenlex.check().unwrap();
    }

    /// check const pool
    fn check_pool<T>(v: Vec<T>, pool_be_checked: &Pool<T>)
    where
        T: Eq + Hash + Clone + Display + Debug,
    {
        let mut testpool: HashSet<T> = HashSet::new();
        for i in &v {
            testpool.insert((*i).clone());
        }
        assert_eq!(testpool.len(), pool_be_checked.len());
        for i in &testpool {
            assert!(
                pool_be_checked.contains_key(i),
                "{} not in pool.{:?} is expected pool\n{:?} is checked pool",
                i,
                testpool,
                pool_be_checked
            );
        }
    }

    #[test]
    fn test_numberlex() {
        gen_test_token_env!(
            r#",,.,100
        
        
        123.9 232_304904
        0b011
        0x2aA4
        0o2434 0 0 1e9 1.2e1 8e-1 18E-4 1.7e-2 1.98e2"#,
            t
        );
        check(
            &mut t,
            vec![
                Token::new(TokenType::Comma, None),
                Token::new(TokenType::Comma, None),
                Token::new(TokenType::Dot, None),
                Token::new(TokenType::Comma, None),
                Token::new(TokenType::IntValue, Some(2)),
                Token::new(TokenType::FloatValue, Some(0)),
                Token::new(TokenType::IntValue, Some(3)),
                Token::new(TokenType::IntValue, Some(4)),
                Token::new(TokenType::IntValue, Some(5)),
                Token::new(TokenType::IntValue, Some(6)),
                Token::new(TokenType::IntValue, Some(INT_VAL_POOL_ZERO)),
                Token::new(TokenType::IntValue, Some(INT_VAL_POOL_ZERO)),
                Token::new(TokenType::IntValue, Some(7)),
                Token::new(TokenType::FloatValue, Some(1)),
                Token::new(TokenType::FloatValue, Some(2)),
                Token::new(TokenType::FloatValue, Some(3)),
                Token::new(TokenType::FloatValue, Some(4)),
                Token::new(TokenType::FloatValue, Some(5)),
            ],
        );
        check_pool(
            vec![100, 232_304904, 0b011, 0x2aA4, 0o2434, 0, 1, 1e9 as i64],
            &t.compiler_data.const_pool.const_ints,
        );
        check_pool(
            vec![
                Float::new(123, 9, 0),
                Float::new(12, 0, 0),
                Float::new(0, 8, 0),
                Float::new(0, 18, 2),
                Float::new(0, 17, 1),
                Float::new(198, 0, 0),
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
    fn test_comprehensive_lex() {
        gen_test_token_env!(
            r#"
import "p"
func a(int val) -> str {
    if val % 2 == 0 {
        return "even"
    } else {
        return "odd"
    }
}
func main() {
    print("hello world")
    p := a(intinput())
    print(p)
}
        "#,
            t
        );
        check(
            &mut t,
            vec![
                Token::new(TokenType::Import, None),
                Token::new(TokenType::StringValue, Some(0)),
                Token::new(TokenType::Func, None),
                Token::new(TokenType::ID, Some(0)),
                Token::new(TokenType::LeftSmallBrace, None),
                Token::new(TokenType::ID, Some(1)),
                Token::new(TokenType::ID, Some(2)),
                Token::new(TokenType::RightSmallBrace, None),
                Token::new(TokenType::Arrow, None),
                Token::new(TokenType::ID, Some(3)),
                Token::new(TokenType::LeftBigBrace, None),
                Token::new(TokenType::If, None),
                Token::new(TokenType::ID, Some(2)),
                Token::new(TokenType::Mod, None),
                Token::new(TokenType::IntValue, Some(2)),
                Token::new(TokenType::Equal, None),
                Token::new(TokenType::IntValue, Some(INT_VAL_POOL_ZERO)),
                Token::new(TokenType::LeftBigBrace, None),
                Token::new(TokenType::Return, None),
                Token::new(TokenType::StringValue, Some(1)),
                Token::new(TokenType::RightBigBrace, None),
                Token::new(TokenType::Else, None),
                Token::new(TokenType::LeftBigBrace, None),
                Token::new(TokenType::Return, None),
                Token::new(TokenType::StringValue, Some(2)),
                Token::new(TokenType::RightBigBrace, None),
                Token::new(TokenType::RightBigBrace, None),
                Token::new(TokenType::Func, None),
                Token::new(TokenType::ID, Some(4)),
                Token::new(TokenType::LeftSmallBrace, None),
                Token::new(TokenType::RightSmallBrace, None),
                Token::new(TokenType::LeftBigBrace, None),
                Token::new(TokenType::ID, Some(5)),
                Token::new(TokenType::LeftSmallBrace, None),
                Token::new(TokenType::StringValue, Some(3)),
                Token::new(TokenType::RightSmallBrace, None),
                Token::new(TokenType::ID, Some(6)),
                Token::new(TokenType::Store, None),
                Token::new(TokenType::ID, Some(0)),
                Token::new(TokenType::LeftSmallBrace, None),
                Token::new(TokenType::ID, Some(7)),
                Token::new(TokenType::LeftSmallBrace, None),
                Token::new(TokenType::RightSmallBrace, None),
                Token::new(TokenType::RightSmallBrace, None),
                Token::new(TokenType::ID, Some(5)),
                Token::new(TokenType::LeftSmallBrace, None),
                Token::new(TokenType::ID, Some(6)),
                Token::new(TokenType::RightSmallBrace, None),
                Token::new(TokenType::RightBigBrace, None),
            ],
        );
    }

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
        check_pool(vec![0xabc, 0, 1], &t.compiler_data.const_pool.const_ints);
        check_pool(
            vec![String::from("hds")],
            &t.compiler_data.const_pool.name_pool,
        );
    }

    #[test]
    fn test_next_back() {
        gen_test_token_env!(r#":()"#, t);
        let tmp = t.next_token().unwrap();
        assert_eq!(tmp.tp, TokenType::Colon);
        t.next_back(tmp);
        assert_eq!(t.next_token().unwrap().tp, TokenType::Colon);
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
