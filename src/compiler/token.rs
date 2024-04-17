use super::{Context, TokenIo, ValuePool};
use crate::{cfg::FLOAT_OVER_FLOW_LIMIT, compiler::CompilerImpl};
use libcore::*;
use rust_i18n::t;
use std::{cell::RefCell, collections::HashMap, fmt::Display, rc::Rc, sync::OnceLock};

#[derive(PartialEq, Debug, Clone, Hash, Eq, Copy)]
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
    CharValue,
    BoolValue,
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
    // 不会被使用到的自反运算符，仅仅当标识重载运算符使用
    SelfNegative,
    EndOfFile,
    Continue,
    Break,
    Var,
    Pub,
    // ::
    DoubleColon,
}

impl TokenType {
    pub fn convert_to_override(&self) -> Option<OverrideOperations> {
        Some(match *self {
            TokenType::Add => OverrideOperations::Add,
            TokenType::Sub => OverrideOperations::Sub,
            TokenType::Mul => OverrideOperations::Mul,
            TokenType::Div => OverrideOperations::Div,
            TokenType::Mod => OverrideOperations::Mod,
            TokenType::ExactDiv => OverrideOperations::ExactDiv,
            TokenType::Power => OverrideOperations::Power,
            TokenType::Not => OverrideOperations::Not,
            TokenType::And => OverrideOperations::And,
            TokenType::Or => OverrideOperations::Or,
            TokenType::SelfNegative => OverrideOperations::SelfNegative,
            TokenType::GreaterEqual => OverrideOperations::GreaterEqual,
            TokenType::Greater => OverrideOperations::Greater,
            TokenType::LessEqual => OverrideOperations::LessEqual,
            TokenType::Less => OverrideOperations::Less,
            TokenType::Equal => OverrideOperations::Equal,
            TokenType::NotEqual => OverrideOperations::NotEqual,
            TokenType::BitNot => OverrideOperations::BitNot,
            TokenType::BitRightShift => OverrideOperations::BitRightShift,
            TokenType::BitLeftShift => OverrideOperations::BitLeftShift,
            TokenType::BitAnd => OverrideOperations::BitAnd,
            TokenType::BitOr => OverrideOperations::BitOr,
            TokenType::Xor => OverrideOperations::Xor,
            _ => return None,
        })
    }
}

pub type ConstPoolIndexTy = usize;

impl Display for TokenType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let res = match self {
            TokenType::Dot => ".",
            TokenType::Comma => ",",
            TokenType::LeftBigBrace => "{",
            TokenType::RightBigBrace => "}",
            TokenType::LeftMiddleBrace => "[",
            TokenType::RightMiddleBrace => "]",
            TokenType::LeftSmallBrace => "(",
            TokenType::RightSmallBrace => ")",
            TokenType::Add => "+",
            TokenType::Sub => "-",
            TokenType::Mul => "*",
            TokenType::Div => "/",
            TokenType::Mod => "%",
            TokenType::ExactDiv => "//",
            TokenType::BitNot => "~",
            TokenType::BitLeftShift => "<<",
            TokenType::BitRightShift => ">>",
            TokenType::BitAnd => "&",
            TokenType::BitOr => "|",
            TokenType::Xor => "^",
            TokenType::Power => "**",
            TokenType::SelfAdd => "+=",
            TokenType::SelfSub => "-=",
            TokenType::SelfMul => "*=",
            TokenType::SelfDiv => "/=",
            TokenType::SelfExactDiv => "//=",
            TokenType::SelfMod => "%=",
            TokenType::SelfPower => "**=",
            TokenType::SelfBitLeftShift => "<<=",
            TokenType::SelfBitRightShift => ">>=",
            TokenType::SelfBitAnd => "&=",
            TokenType::SelfBitOr => "|=",
            TokenType::SelfXor => "^=",
            TokenType::IntValue => "integer",
            TokenType::StringValue => "string",
            TokenType::FloatValue => "float",
            TokenType::LongIntValue => "long integer",
            TokenType::Assign => "=",
            TokenType::Store => ":=",
            TokenType::Equal => "==",
            TokenType::NotEqual => "!=",
            TokenType::Greater => ">",
            TokenType::Less => "<",
            TokenType::LessEqual => "<=",
            TokenType::GreaterEqual => ">=",
            TokenType::Not => "!",
            TokenType::Or => "||",
            TokenType::And => "&&",
            TokenType::Colon => ":",
            TokenType::Semicolon => ";",
            TokenType::ID => "identifier",
            TokenType::While => "while",
            TokenType::For => "for",
            TokenType::If => "if",
            TokenType::Else => "else",
            TokenType::Class => "class",
            TokenType::Match => "match",
            TokenType::Func => "func",
            TokenType::EndOfFile => "EOF",
            TokenType::Import => "import",
            TokenType::Arrow => "->",
            TokenType::Return => "return",
            TokenType::SelfAnd => "&&=",
            TokenType::SelfOr => "||=",
            TokenType::SelfNegative => "-",
            TokenType::CharValue => "char",
            TokenType::BoolValue => "bool",
            TokenType::Continue => "continue",
            TokenType::Break => "break",
            TokenType::Var => "var",
            TokenType::Pub => "pub",
            TokenType::DoubleColon => "::",
        };
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

pub struct TokenLex {
    compiler_data: Rc<RefCell<CompilerImpl>>,
    braces_check: Vec<BraceRecord>,
    // token和当前行号
    unget_token: Vec<(Token, usize)>,
    pub const_pool: ValuePool,
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
                        let tmp = &$sself.compiler_data.borrow().context.module_name.clone();
                        return $sself.report_error_with_context(RuntimeError::new(
                            Box::new(Context::new_line(tmp, $brace_record.line)),
                            ErrorInfo::new(
                                t!(UNMATCHED_BRACE, "0"=$brace_record.c),
                                t!(SYNTAX_ERROR),
                            ),
                        ));
                    }
                },
            )*
            _ => {
               unreachable!()
            }
        }
    }}
}

fn get_keywords() -> &'static HashMap<String, TokenType> {
    static KEYWORDS: OnceLock<HashMap<String, TokenType>> = OnceLock::new();
    KEYWORDS.get_or_init(|| {
        collection_literals::hash![
            "while".to_string() => TokenType::While,
            "for".to_string() => TokenType::For,
            "if".to_string() => TokenType::If,
            "else".to_string() => TokenType::Else,
            "class".to_string() => TokenType::Class,
            "func".to_string() => TokenType::Func,
            "match".to_string() => TokenType::Match,
            "return".to_string() => TokenType::Return,
            "import".to_string() => TokenType::Import,
            "continue".to_string() => TokenType::Continue,
            "break".to_string() => TokenType::Break,
            "var".to_string() => TokenType::Var,
            "pub".to_string() => TokenType::Pub
        ]
    })
}

fn get_redix_to_prefix() -> &'static HashMap<usize, &'static str> {
    static RADIX_TO_PREFIX: OnceLock<HashMap<usize, &'static str>> = OnceLock::new();
    RADIX_TO_PREFIX.get_or_init(|| {
        collection_literals::hash![
            2 => "0b",
            8 => "0o",
            16 => "0x"
        ]
    })
}

enum NumValue {
    Integer(i64),
    FloatVal(String),
}

impl Iterator for TokenLex {
    type Item = Token;

    fn next(&mut self) -> Option<Self::Item> {
        match self.next_token() {
            Ok(v) => {
                if v.tp == TokenType::EndOfFile {
                    None
                } else {
                    Some(v)
                }
            }
            Err(_) => None,
        }
    }
}

impl TokenLex {
    pub fn new(compiler_data: Rc<RefCell<CompilerImpl>>) -> TokenLex {
        TokenLex {
            compiler_data,
            braces_check: Vec::new(),
            unget_token: Vec::new(),
            const_pool: ValuePool::new(),
        }
    }

    pub fn add_id_token(&mut self, id_name: &str) -> usize {
        self.const_pool.add_id(id_name.to_string())
    }

    fn check_braces_stack(&mut self, c: char) -> Result<(), RuntimeError> {
        let top = self.braces_check.pop();
        match top {
            None => self.report_error(ErrorInfo::new(
                t!(UNMATCHED_BRACE, "0" = c),
                t!(SYNTAX_ERROR),
            )),
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

    pub fn modify_input(&mut self, source: Box<dyn TokenIo<Item = char>>) {
        self.compiler_data.borrow_mut().input = source;
    }

    fn lex_id(&mut self, c: char) -> RuntimeResult<Token> {
        Ok({
            let mut retname: String = String::from(c);
            loop {
                let c = self.compiler_data.borrow_mut().input.read();
                if Self::is_id_char(c) {
                    retname.push(c);
                } else {
                    self.compiler_data.borrow_mut().input.unread(c);
                    break;
                }
            }
            let tmp = get_keywords().get(&retname);
            match tmp {
                Some(val) => Token::new(*val, None),
                None => {
                    if retname == "true" {
                        return Ok(Token::new(TokenType::BoolValue, Some(1)));
                    }
                    if retname == "false" {
                        return Ok(Token::new(TokenType::BoolValue, Some(0)));
                    }
                    Token::new(TokenType::ID, Some(self.const_pool.add_id(retname)))
                }
            }
        })
    }

    fn check_whether_symbol(c: char) -> bool {
        matches!(
            c,
            '.' | ','
                | '{'
                | '}'
                | '['
                | ']'
                | '('
                | ')'
                | '+'
                | '-'
                | '*'
                | '%'
                | '/'
                | '='
                | '!'
                | '>'
                | '<'
                | '~'
                | '^'
                | '|'
                | ':'
                | ';'
                | '&'
        )
    }

    fn is_useless_char(c: char) -> bool {
        matches!(c, ' ' | '\n' | '\t' | '\0')
    }

    fn is_string_or_char_begin(c: char) -> bool {
        matches!(c, '"' | '\'')
    }

    fn is_id_char(c: char) -> bool {
        !(Self::check_whether_symbol(c)
            || Self::is_string_or_char_begin(c)
            || Self::is_useless_char(c))
    }

    fn lex_symbol(&mut self, c: char) -> RuntimeResult<Token> {
        Ok(match c {
            '.' => Token::new(TokenType::Dot, None),
            ',' => Token::new(TokenType::Comma, None),
            '{' => {
                self.braces_check.push(BraceRecord::new(
                    c,
                    self.compiler_data.borrow().context.get_line(),
                ));
                Token::new(TokenType::LeftBigBrace, None)
            }
            '}' => {
                self.check_braces_stack(c)?;
                Token::new(TokenType::RightBigBrace, None)
            }
            '[' => {
                self.braces_check.push(BraceRecord::new(
                    c,
                    self.compiler_data.borrow().context.get_line(),
                ));
                Token::new(TokenType::LeftMiddleBrace, None)
            }
            ']' => {
                self.check_braces_stack(c)?;
                Token::new(TokenType::RightMiddleBrace, None)
            }
            '(' => {
                self.braces_check.push(BraceRecord::new(
                    c,
                    self.compiler_data.borrow().context.get_line(),
                ));
                Token::new(TokenType::LeftSmallBrace, None)
            }
            ')' => {
                self.check_braces_stack(c)?;
                Token::new(TokenType::RightSmallBrace, None)
            }
            '+' => self.self_symbol(TokenType::Add, TokenType::SelfAdd),
            '-' => {
                let c = self.compiler_data.borrow_mut().input.read();
                if c == '>' {
                    Token::new(TokenType::Arrow, None)
                } else if c == '=' {
                    Token::new(TokenType::SelfSub, None)
                } else {
                    self.compiler_data.borrow_mut().input.unread(c);
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
            '/' => {
                // 特判注释
                let c = self.compiler_data.borrow_mut().input.read();
                if c == '*' {
                    loop {
                        let c = self.compiler_data.borrow_mut().input.read();
                        if c == '*' {
                            let c = self.compiler_data.borrow_mut().input.read();
                            if c == '/' {
                                return self.next_token();
                            }
                            self.compiler_data.borrow_mut().input.unread(c);
                        } else if c == '\0' {
                            return self.report_error(ErrorInfo::new(
                                t!(UNCLODED_COMMENT),
                                t!(SYNTAX_ERROR),
                            ));
                        } else if c == '\n' {
                            self.compiler_data.borrow_mut().context.add_line();
                        }
                    }
                }
                self.compiler_data.borrow_mut().input.unread(c);
                self.double_symbol(
                    TokenType::Div,
                    TokenType::SelfDiv,
                    TokenType::ExactDiv,
                    TokenType::SelfExactDiv,
                    '/',
                )
            }
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
            ':' => {
                let c = self.compiler_data.borrow_mut().input.read();
                if c == '=' {
                    Token::new(TokenType::Store, None)
                } else if c == ':' {
                    Token::new(TokenType::DoubleColon, None)
                } else {
                    self.compiler_data.borrow_mut().input.unread(c);
                    Token::new(TokenType::Colon, None)
                }
            }
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
                unreachable!()
            }
        })
    }

    /// lex only an integer
    fn lex_num_integer(&mut self, c: char, radix: u32) -> String {
        let mut s = String::from(c);
        let mut presecnt_lex;
        loop {
            presecnt_lex = self.compiler_data.borrow_mut().input.read();
            if presecnt_lex == '_' {
                continue;
            }
            if presecnt_lex.is_digit(radix) {
                s.push(presecnt_lex);
            } else {
                self.compiler_data.borrow_mut().input.unread(presecnt_lex);
                break;
            }
        }
        s
    }

    fn lex_int_float(&mut self, mut c: char) -> RuntimeResult<NumValue> {
        // the radix of result
        let mut radix = 10;
        if c == '0' {
            // check the radix
            c = self.compiler_data.borrow_mut().input.read();
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
                    self.compiler_data.borrow_mut().input.unread(c);
                    return Ok(NumValue::Integer(0));
                }
            }
            c = self.compiler_data.borrow_mut().input.read();
        }
        let intpart = self.lex_num_integer(c, radix).to_string();
        c = self.compiler_data.borrow_mut().input.read();
        if c == '.' {
            // float can be used with prefix
            if radix != 10 {
                return self.report_error(ErrorInfo::new(
                    t!(
                        PREFIX_FOR_FLOAT,
                        "0" = get_redix_to_prefix()[&(radix as usize)]
                    ),
                    t!(SYNTAX_ERROR),
                ));
            }
            // float mode
            c = self.compiler_data.borrow_mut().input.read();
            let float_part = self.lex_num_integer(c, radix);
            if float_part.len() + intpart.len() > FLOAT_OVER_FLOW_LIMIT {
                // overflow
                return self.report_error(ErrorInfo::new(
                    t!(FLOAT_OVER_FLOW, "0" = format!("{intpart}.{float_part}")),
                    t!(NUMBER_OVER_FLOW),
                ));
            }
            return Ok(NumValue::FloatVal(format!(
                "{}.{}",
                i64::from_str_radix(&intpart, radix).unwrap().to_string(),
                i64::from_str_radix(&float_part, radix).unwrap().to_string(),
            )));
        } else {
            self.compiler_data.borrow_mut().input.unread(c);
        }
        Ok(NumValue::Integer(
            i64::from_str_radix(&intpart, radix).unwrap(),
        ))
    }

    fn turn_to_token(&mut self, val: NumValue) -> Token {
        match val {
            NumValue::FloatVal(v) => {
                Token::new(TokenType::FloatValue, Some(self.const_pool.add_float(v)))
            }
            NumValue::Integer(it) => {
                Token::new(TokenType::IntValue, Some(self.const_pool.add_int(it)))
            }
        }
    }

    fn lex_num(&mut self, mut c: char) -> RuntimeResult<Token> {
        let tmp = self.lex_int_float(c)?;
        c = self.compiler_data.borrow_mut().input.read();
        if c == 'e' || c == 'E' {
            // 科学计数法
            c = self.compiler_data.borrow_mut().input.read();
            // 科学计数法的指数符号
            let mut up_flag: i64 = 1;
            if c == '+' {
                c = self.compiler_data.borrow_mut().input.read();
            } else if c == '-' {
                up_flag = -1;
                c = self.compiler_data.borrow_mut().input.read();
            }
            // 读取科学计数法的指数部分
            let mut up: i64 = self.lex_num_integer(c, 10).parse().unwrap();
            up *= up_flag;
            match tmp {
                NumValue::Integer(mut it) => {
                    if up >= 0 {
                        // 保留int身份
                        for _ in 0..up {
                            it *= 10;
                        }
                        Ok(Token::new(
                            TokenType::IntValue,
                            Some(self.const_pool.add_int(it)),
                        ))
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
                        Ok(Token::new(
                            TokenType::FloatValue,
                            Some(self.const_pool.add_float(format!("{}.{}", it, float_part))),
                        ))
                    }
                }
                NumValue::FloatVal(v) => {
                    // 转换为两个元素的元组
                    let mut vv = v.split('.');
                    let mut v = (vv.next().unwrap().to_owned(), vv.next().unwrap().to_owned());
                    debug_assert_eq!(vv.next(), None);
                    if up >= 0 {
                        for _ in 0..up {
                            if v.1.is_empty() {
                                v.0.push('0');
                            } else {
                                v.0.push(v.1.remove(0));
                            }
                        }
                        let ret = if v.1.is_empty() {
                            v.0
                        } else {
                            format!("{}.{}", v.0, v.1)
                        };
                        Ok(Token::new(
                            TokenType::FloatValue,
                            Some(self.const_pool.add_float(ret)),
                        ))
                    } else {
                        up = -up;
                        let mut s = String::new();
                        let mut zero_num = 0;
                        for _ in 0..up {
                            if v.0.is_empty() {
                                zero_num += 1;
                            } else {
                                s = (v.0.pop().unwrap()).to_string() + &s;
                            }
                        }
                        if v.0.is_empty() {
                            v.0 = "0".to_string();
                        }
                        s = format!("{}.{}{}", v.0, "0".repeat(zero_num), s + &v.1.to_string());
                        Ok(Token::new(
                            TokenType::FloatValue,
                            Some(self.const_pool.add_float(s)),
                        ))
                    }
                }
            }
        } else {
            self.compiler_data.borrow_mut().input.unread(c);
            Ok(self.turn_to_token(tmp))
        }
    }

    fn lex_str(&mut self) -> RuntimeResult<Token> {
        let mut s = String::new();
        let mut c = self.compiler_data.borrow_mut().input.read();
        while c != '"' {
            if c == '\\' {
                c = self.compiler_data.borrow_mut().input.read();
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
            c = self.compiler_data.borrow_mut().input.read();
            if c == '\0' {
                return self.report_error(ErrorInfo::new(t!(STRING_WITHOUT_END), t!(SYNTAX_ERROR)));
            }
        }
        Ok(Token::new(
            TokenType::StringValue,
            Some(self.const_pool.add_string(s)),
        ))
    }

    fn lex_char(&mut self) -> RuntimeResult<Token> {
        let c = self.compiler_data.borrow_mut().input.read();
        let end = self.compiler_data.borrow_mut().input.read();
        if end != '\'' {
            return self.report_error(ErrorInfo::new(t!(CHAR_FORMAT), t!(SYNTAX_ERROR)));
        }
        Ok(Token::new(TokenType::CharValue, Some(c as usize)))
    }

    fn report_error<T>(&mut self, e: ErrorInfo) -> Result<T, RuntimeError> {
        self.clear_error();
        self.compiler_data.borrow().report_compiler_error(e)
    }

    fn report_error_with_context<T>(&mut self, e: RuntimeError) -> Result<T, RuntimeError> {
        self.clear_error();
        Err(e)
    }

    pub fn clear_error(&mut self) {
        self.braces_check.clear();
        self.unget_token.clear();
    }

    pub fn next_token(&mut self) -> RuntimeResult<Token> {
        if !self.unget_token.is_empty() {
            let tmp = self.unget_token.pop().unwrap();
            self.compiler_data.borrow_mut().context.set_line(tmp.1);
            return Ok(tmp.0);
        }
        let presecnt_lex = self.compiler_data.borrow_mut().input.read();
        match presecnt_lex {
            '\0' => {
                return Ok(Token::new(TokenType::EndOfFile, None));
            }
            '\t' | ' ' => {
                return self.next_token();
            }
            '\n' => {
                self.compiler_data.borrow_mut().context.add_line();
                return self.next_token();
            }
            '#' => {
                // 注释
                loop {
                    let c = self.compiler_data.borrow_mut().input.read();
                    if c == '\n' {
                        self.compiler_data.borrow_mut().context.add_line();
                        return self.next_token();
                    }
                    if c == '\0' {
                        return Ok(Token::new(TokenType::EndOfFile, None));
                    }
                }
            }
            _ => {}
        }
        if presecnt_lex.is_ascii_digit() {
            return self.lex_num(presecnt_lex);
        }
        if presecnt_lex == '"' {
            return self.lex_str();
        }
        if presecnt_lex == '\'' {
            return self.lex_char();
        }
        if Self::check_whether_symbol(presecnt_lex) {
            return self.lex_symbol(presecnt_lex);
        }
        self.lex_id(presecnt_lex)
    }

    pub fn next_back(&mut self, t: Token) {
        self.unget_token
            .push((t, self.compiler_data.borrow().context.get_line()));
    }

    /// 回退并重新设置行号
    pub fn next_back_with_line(&mut self, t: Token, line_num: usize) {
        self.next_back(t);
        self.compiler_data.borrow_mut().context.set_line(line_num);
    }

    pub fn check(&mut self) -> Result<(), RuntimeError> {
        if !self.braces_check.is_empty() {
            let unmatch_char = self.braces_check.pop().unwrap();
            let tmp = self.compiler_data.borrow_mut().context.module_name.clone();
            return self.report_error_with_context(RuntimeError::new(
                Box::new(Context::new_line(&(tmp), unmatch_char.line)),
                ErrorInfo::new(t!(UNMATCHED_BRACE, "0" = unmatch_char.c), t!(SYNTAX_ERROR)),
            ));
        }
        Ok(())
    }

    fn binary_symbol(&mut self, a: TokenType, b: TokenType, binary_sym: char) -> Token {
        let c = self.compiler_data.borrow_mut().input.read();
        if c == binary_sym {
            Token::new(b, None)
        } else {
            self.compiler_data.borrow_mut().input.unread(c);
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
        let c = self.compiler_data.borrow_mut().input.read();
        if c == matched_char {
            self.self_symbol(matched_sym, matched_self_sym)
        } else {
            self.compiler_data.borrow_mut().input.unread(c);
            self.self_symbol(before_sym, before_self_sym)
        }
    }
}

#[cfg(test)]
mod tests {
    use crate::compiler::{
        CompileOption, Compiler, InputSource, Pool, INT_VAL_POOL_ONE, INT_VAL_POOL_ZERO,
    };
    use std::{collections::HashSet, fmt::Debug, hash::Hash};

    use super::*;

    macro_rules! gen_test_token_env {
        ($test_string:expr, $env_name:ident) => {
            let env = Compiler::new_string_compiler(
                CompileOption::new(false, InputSource::StringInternal),
                $test_string,
            );
            let mut $env_name = TokenLex::new(env.compiler_impl.clone());
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
    fn check_pool<T, U>(v: Vec<U>, pool_be_checked: &Pool<T>)
    where
        T: Eq + Hash + Clone + Display + Debug + std::convert::From<U>,
        U: Eq + Hash + Clone + Display + Debug + Into<T>,
    {
        let mut testpool: HashSet<T> = HashSet::new();
        for i in &v {
            testpool.insert(((*i).clone()).into());
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

    fn check_until_eof(tokenlex: &mut TokenLex) {
        loop {
            let token_tmp = tokenlex.next_token().unwrap();
            println!("{:?}", token_tmp);
            if token_tmp.tp == TokenType::EndOfFile {
                break;
            }
        }
        // println!("fuck you ccf");
        tokenlex.check().unwrap();
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
            vec![100, 232_304904, 0b011, 0x2AA4, 0o2434, 0, 1, 1e9 as i64],
            &t.const_pool.const_ints,
        );
        check_pool(
            vec!["123.9", "12", "0.8", "0.0018", "0.017", "198"],
            &t.const_pool.const_floats,
        );
    }

    #[test]
    fn test_symbol_lex() {
        gen_test_token_env!(
            r#":{}[]()+=%=//= // /=** *=*,
    >><< >>=||&&:::=:"#,
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
                Token::new(TokenType::Or, None),
                Token::new(TokenType::And, None),
                Token::new(TokenType::DoubleColon, None),
                Token::new(TokenType::Store, None),
                Token::new(TokenType::Colon, None),
            ],
        );
    }

    #[test]
    fn test_string_lex() {
        gen_test_token_env!(r#""s"'s'"sdscdcdfvf""depkd""""\n\t""ttt\tt""#, t);
        check(
            &mut t,
            vec![
                Token::new(TokenType::StringValue, Some(0)),
                Token::new(TokenType::CharValue, Some('s' as usize)),
                Token::new(TokenType::StringValue, Some(1)),
                Token::new(TokenType::StringValue, Some(2)),
                Token::new(TokenType::StringValue, Some(3)),
                Token::new(TokenType::StringValue, Some(4)),
                Token::new(TokenType::StringValue, Some(5)),
            ],
        );
        check_pool(
            vec![
                String::from("s"),
                String::from("sdscdcdfvf"),
                String::from("depkd"),
                String::from(""),
                String::from("\n\t"),
                String::from("ttt\tt"),
            ],
            &t.const_pool.const_strings,
        );
    }

    #[test]
    fn test_comprehensive_lex() {
        gen_test_token_env!(
            r#"
            /*a complex test*
             *
             *
             * end */
import "p"
func a(int val) -> str {
    if val % 2 == 0 {
        return "even"
    } else {
        return "odd"
    }
}
#djopekdpekdpedle
func main() {
    print("hello world")#djeopjdfopejfopejfpejfop
    p := a(intinput())
    print(p)
}
#ojdeopjfoepjfopejop
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
        gen_test_token_env!(r#"id fuck _fuck 天帝abc abc天帝 t1"#, t);
        check(
            &mut t,
            vec![
                Token::new(TokenType::ID, Some(0)),
                Token::new(TokenType::ID, Some(1)),
                Token::new(TokenType::ID, Some(2)),
                Token::new(TokenType::ID, Some(3)),
                Token::new(TokenType::ID, Some(4)),
                Token::new(TokenType::ID, Some(5)),
            ],
        );
        check_pool(
            vec![
                String::from("id"),
                String::from("fuck"),
                String::from("_fuck"),
                String::from("天帝abc"),
                String::from("abc天帝"),
                String::from("t1"),
            ],
            &t.const_pool.name_pool,
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
        check_pool(vec![0b1, 23, 0], &t.const_pool.const_ints);
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
        check_pool(vec![0xabc, 0, 1], &t.const_pool.const_ints);
        check_pool(vec!["hds".to_string()], &t.const_pool.name_pool);
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
    #[should_panic(expected = "char")]
    fn test_wrong_char1() {
        gen_test_token_env!(r#"''"#, t);
        check_until_eof(&mut t);
    }

    #[test]
    #[should_panic(expected = "char")]
    fn test_wrong_char2() {
        gen_test_token_env!(r#"'sasa'"#, t);
        check_until_eof(&mut t);
    }

    #[test]
    #[should_panic(expected = "char")]
    fn test_wrong_char3() {
        gen_test_token_env!(r#"'"#, t);
        check_until_eof(&mut t);
    }

    #[test]
    fn test_bool() {
        gen_test_token_env!(r#"true tru false"#, t);
        check(
            &mut t,
            vec![
                Token::new(TokenType::BoolValue, Some(1)),
                Token::new(TokenType::ID, Some(0)),
                Token::new(TokenType::BoolValue, Some(0)),
            ],
        )
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

    #[test]
    #[should_panic(expected = "char")]
    fn test_error_char() {
        gen_test_token_env!(r#"print("{}", 'pp')"#, t);
        check_until_eof(&mut t);
    }

    #[test]
    #[should_panic(expected = "string")]
    fn test_error_str() {
        gen_test_token_env!(r#"print("{}", "pp)"#, t);
        check_until_eof(&mut t);
    }
}
