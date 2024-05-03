use super::{Context, TokenIo, ValuePool};
use crate::compiler::CompilerImpl;
use libcore::*;
use logos::{Lexer, Logos};
use rust_i18n::t;
use std::{cell::RefCell, collections::HashMap, fmt::Display, rc::Rc, sync::OnceLock};

fn add_float(lex: &mut Lexer<Token>) -> usize {
    lex.extras.add_float(lex.slice().to_owned())
}

fn add_string(lex: &mut Lexer<Token>) -> usize {
    lex.extras.add_string(lex.slice().to_owned())
}

fn add_id(lex: &mut Lexer<Token>) -> usize {
    lex.extras.add_id(lex.slice().to_owned())
}

fn convert_int(lex: &mut Lexer<Token>) -> usize {
    let val: i64 = lex.slice().parse().unwrap();
    convert_int_constval_to_usize(val)
}

#[derive(PartialEq, Debug, Clone, Hash, Eq, Copy, Logos)]
#[logos(extras = ValuePool)]
#[logos(skip r"[ \t\r\n\f]+")]
pub enum Token {
    #[token("->")]
    Arrow,
    #[token(".")]
    Dot,
    #[token(",")]
    Comma,
    #[token("{")]
    LeftBigBrace,
    #[token("}")]
    RightBigBrace,
    #[token("[")]
    LeftMiddleBrace,
    #[token("]")]
    RightMiddleBrace,
    #[token("(")]
    LeftSmallBrace,
    #[token(")")]
    RightSmallBrace,
    #[token("+")]
    Add,
    #[token("-")]
    Sub,
    #[token("*")]
    Mul,
    #[token("/")]
    Div,
    #[token("%")]
    Mod,
    #[token("//")]
    ExactDiv,
    #[token("~")]
    BitNot,
    #[token("<<")]
    BitLeftShift,
    #[token(">>")]
    BitRightShift,
    #[token("&")]
    BitAnd,
    #[token("|")]
    BitOr,
    #[token("^")]
    Xor,
    #[token("**")]
    Power,
    #[token("+=")]
    SelfAdd,
    #[token("-=")]
    SelfSub,
    #[token("*=")]
    SelfMul,
    #[token("/=")]
    SelfDiv,
    #[token("//=")]
    SelfExactDiv,
    #[token("%=")]
    SelfMod,
    #[token("**=")]
    SelfPower,
    #[token("<<=")]
    SelfBitLeftShift,
    #[token(">>=")]
    SelfBitRightShift,
    #[token("&=")]
    SelfBitAnd,
    #[token("|=")]
    SelfBitOr,
    #[token("^=")]
    SelfXor,
    #[regex(r#"[+-]?(0[bB][01]+|0[oO][0-7]+|0[xX][0-9a-fA-F]+|\d+)"#, convert_int)]
    IntValue(usize),
    #[regex(r#"".*""#, add_string)]
    StringValue(usize),
    #[regex(r#"[-+]?[0-9]+\.?[0-9]+([eE][-+]?[0-9]+)?"#, add_float)]
    FloatValue(usize),
    LongIntValue,
    #[regex(r#"'[a-zA-Z_0-9]'"#, |lex| lex.slice().chars().next().unwrap() as usize)]
    CharValue(usize),
    #[token("false", |_| false as usize)]
    #[token("true", |_| true as usize)]
    BoolValue(usize),
    #[token("||=")]
    SelfOr,
    #[token("&&=")]
    SelfAnd,
    #[token("=")]
    Assign,
    #[token(":=")]
    Store,
    #[token("==")]
    Equal,
    #[token("!=")]
    NotEqual,
    #[token(">")]
    Greater,
    #[token("<")]
    Less,
    #[token("<=")]
    LessEqual,
    #[token(">=")]
    GreaterEqual,
    #[token("!")]
    Not,
    #[token("||")]
    Or,
    #[token("&&")]
    And,
    #[token(":")]
    Colon,
    #[token(";")]
    Semicolon,
    #[regex(r#"[a-zA-Z_][a-zA-Z_0-9]*"#, add_id)]
    ID(usize),
    #[token("while")]
    While,
    #[token("for")]
    For,
    #[token("if")]
    If,
    #[token("else")]
    Else,
    #[token("class")]
    Class,
    #[token("match")]
    Match,
    #[token("func")]
    Func,
    #[token("import")]
    Import,
    #[token("return")]
    Return,
    // 不会被使用到的自反运算符，仅仅当标识重载运算符使用
    SelfNegative,
    #[token("\0")]
    EndOfFile,
    #[token("continue")]
    Continue,
    #[token("break")]
    Break,
    #[token("var")]
    Var,
    #[token("pub")]
    Pub,
    #[token("::")]
    DoubleColon,
    #[token("uninit")]
    Uninit,
    #[token("unsafe")]
    Unsafe,
}

impl Token {
    pub fn convert_to_override(&self) -> Option<OverrideOperations> {
        Some(match *self {
            Token::Add => OverrideOperations::Add,
            Token::Sub => OverrideOperations::Sub,
            Token::Mul => OverrideOperations::Mul,
            Token::Div => OverrideOperations::Div,
            Token::Mod => OverrideOperations::Mod,
            Token::ExactDiv => OverrideOperations::ExactDiv,
            Token::Power => OverrideOperations::Power,
            Token::Not => OverrideOperations::Not,
            Token::And => OverrideOperations::And,
            Token::Or => OverrideOperations::Or,
            Token::SelfNegative => OverrideOperations::SelfNegative,
            Token::GreaterEqual => OverrideOperations::GreaterEqual,
            Token::Greater => OverrideOperations::Greater,
            Token::LessEqual => OverrideOperations::LessEqual,
            Token::Less => OverrideOperations::Less,
            Token::Equal => OverrideOperations::Equal,
            Token::NotEqual => OverrideOperations::NotEqual,
            Token::BitNot => OverrideOperations::BitNot,
            Token::BitRightShift => OverrideOperations::BitRightShift,
            Token::BitLeftShift => OverrideOperations::BitLeftShift,
            Token::BitAnd => OverrideOperations::BitAnd,
            Token::BitOr => OverrideOperations::BitOr,
            Token::Xor => OverrideOperations::Xor,
            _ => return None,
        })
    }
}

pub type ConstPoolIndexTy = usize;

impl Display for Token {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let res = match self {
            Token::Dot => ".",
            Token::Comma => ",",
            Token::LeftBigBrace => "{",
            Token::RightBigBrace => "}",
            Token::LeftMiddleBrace => "[",
            Token::RightMiddleBrace => "]",
            Token::LeftSmallBrace => "(",
            Token::RightSmallBrace => ")",
            Token::Add => "+",
            Token::Sub => "-",
            Token::Mul => "*",
            Token::Div => "/",
            Token::Mod => "%",
            Token::ExactDiv => "//",
            Token::BitNot => "~",
            Token::BitLeftShift => "<<",
            Token::BitRightShift => ">>",
            Token::BitAnd => "&",
            Token::BitOr => "|",
            Token::Xor => "^",
            Token::Power => "**",
            Token::SelfAdd => "+=",
            Token::SelfSub => "-=",
            Token::SelfMul => "*=",
            Token::SelfDiv => "/=",
            Token::SelfExactDiv => "//=",
            Token::SelfMod => "%=",
            Token::SelfPower => "**=",
            Token::SelfBitLeftShift => "<<=",
            Token::SelfBitRightShift => ">>=",
            Token::SelfBitAnd => "&=",
            Token::SelfBitOr => "|=",
            Token::SelfXor => "^=",
            Token::IntValue(_) => "integer",
            Token::StringValue(_) => "string",
            Token::FloatValue(_) => "float",
            Token::LongIntValue => "long integer",
            Token::Assign => "=",
            Token::Store => ":=",
            Token::Equal => "==",
            Token::NotEqual => "!=",
            Token::Greater => ">",
            Token::Less => "<",
            Token::LessEqual => "<=",
            Token::GreaterEqual => ">=",
            Token::Not => "!",
            Token::Or => "||",
            Token::And => "&&",
            Token::Colon => ":",
            Token::Semicolon => ";",
            Token::ID(_) => "identifier",
            Token::While => "while",
            Token::For => "for",
            Token::If => "if",
            Token::Else => "else",
            Token::Class => "class",
            Token::Match => "match",
            Token::Func => "func",
            Token::EndOfFile => "EOF",
            Token::Import => "import",
            Token::Arrow => "->",
            Token::Return => "return",
            Token::SelfAnd => "&&=",
            Token::SelfOr => "||=",
            Token::SelfNegative => "-",
            Token::CharValue(_) => "char",
            Token::BoolValue(_) => "bool",
            Token::Continue => "continue",
            Token::Break => "break",
            Token::Var => "var",
            Token::Pub => "pub",
            Token::DoubleColon => "::",
            Token::Uninit => "uninit",
            Token::Unsafe => "unsafe",
        };
        write!(f, "{}", res)
    }
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

fn get_keywords() -> &'static HashMap<String, Token> {
    static KEYWORDS: OnceLock<HashMap<String, Token>> = OnceLock::new();
    KEYWORDS.get_or_init(|| {
        collection_literals::hash![
            "while".to_string() => Token::While,
            "for".to_string() => Token::For,
            "if".to_string() => Token::If,
            "else".to_string() => Token::Else,
            "class".to_string() => Token::Class,
            "func".to_string() => Token::Func,
            "match".to_string() => Token::Match,
            "return".to_string() => Token::Return,
            "import".to_string() => Token::Import,
            "continue".to_string() => Token::Continue,
            "break".to_string() => Token::Break,
            "var".to_string() => Token::Var,
            "pub".to_string() => Token::Pub,
            "unsafe".to_string() => Token::Unsafe,
            "uninit".to_string() => Token::Uninit
        ]
    })
}

impl Iterator for TokenLex {
    type Item = Token;

    fn next(&mut self) -> Option<Self::Item> {
        match self.next_token() {
            Ok(v) => {
                if v == Token::EndOfFile {
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
                return Ok(Token::EndOfFile);
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
                        return Ok(Token::EndOfFile);
                    }
                }
            }
            _ => {}
        }
        todo!()
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
}

#[cfg(test)]
mod tests {
    use crate::compiler::{CompileOption, Compiler, InputSource, Pool};
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
        assert_eq!(Token::EndOfFile, tokenlex.next_token().unwrap());
        tokenlex.check().unwrap();
    }

    /// check const pool
    fn check_pool<T, U>(v: Vec<U>, pool_be_checked: &Pool<T>)
    where
        T: Eq + Hash + Clone + Display + Debug + From<U>,
        U: Eq + Hash + Clone + Display + Debug + Into<T>,
    {
        let mut testpool: HashSet<T> = HashSet::new();
        for i in &v {
            testpool.insert((*i).clone().into());
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
            if token_tmp == Token::EndOfFile {
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
                Token::Comma,
                (Token::Comma),
                (Token::Dot),
                (Token::Comma),
                Token::IntValue(convert_int_constval_to_usize(100)),
                (Token::FloatValue(0)),
                Token::IntValue(convert_int_constval_to_usize(232_304904)),
                Token::IntValue(convert_int_constval_to_usize(0b011)),
                Token::IntValue(convert_int_constval_to_usize(0x2AA4)),
                Token::IntValue(convert_int_constval_to_usize(0o2434)),
                (Token::IntValue(convert_int_constval_to_usize(0))),
                (Token::IntValue(convert_int_constval_to_usize(0))),
                Token::IntValue(convert_int_constval_to_usize(1e9 as TrcIntInternal)),
                (Token::FloatValue(1)),
                (Token::FloatValue(2)),
                (Token::FloatValue(3)),
                (Token::FloatValue(4)),
                (Token::FloatValue(5)),
            ],
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
                (Token::Colon),
                (Token::LeftBigBrace),
                (Token::RightBigBrace),
                (Token::LeftMiddleBrace),
                (Token::RightMiddleBrace),
                (Token::LeftSmallBrace),
                (Token::RightSmallBrace),
                (Token::SelfAdd),
                (Token::SelfMod),
                (Token::SelfExactDiv),
                (Token::ExactDiv),
                (Token::SelfDiv),
                (Token::Power),
                (Token::SelfMul),
                (Token::Mul),
                (Token::Comma),
                (Token::BitRightShift),
                (Token::BitLeftShift),
                (Token::SelfBitRightShift),
                (Token::Or),
                (Token::And),
                (Token::DoubleColon),
                (Token::Store),
                (Token::Colon),
            ],
        );
    }

    #[test]
    fn test_string_lex() {
        gen_test_token_env!(r#""s"'s'"sdscdcdfvf""depkd""""\n\t""ttt\tt""#, t);
        check(
            &mut t,
            vec![
                (Token::StringValue(0)),
                (Token::CharValue('s' as usize)),
                (Token::StringValue(1)),
                (Token::StringValue(2)),
                (Token::StringValue(3)),
                (Token::StringValue(4)),
                (Token::StringValue(5)),
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
                (Token::Import),
                (Token::StringValue(0)),
                (Token::Func),
                (Token::ID(0)),
                (Token::LeftSmallBrace),
                (Token::ID(1)),
                (Token::ID(2)),
                (Token::RightSmallBrace),
                (Token::Arrow),
                (Token::ID(3)),
                (Token::LeftBigBrace),
                (Token::If),
                (Token::ID(2)),
                (Token::Mod),
                (Token::IntValue(2)),
                (Token::Equal),
                (Token::IntValue(convert_int_constval_to_usize(0))),
                (Token::LeftBigBrace),
                (Token::Return),
                (Token::StringValue(1)),
                (Token::RightBigBrace),
                (Token::Else),
                (Token::LeftBigBrace),
                (Token::Return),
                (Token::StringValue(2)),
                (Token::RightBigBrace),
                (Token::RightBigBrace),
                (Token::Func),
                (Token::ID(4)),
                (Token::LeftSmallBrace),
                (Token::RightSmallBrace),
                (Token::LeftBigBrace),
                (Token::ID(5)),
                (Token::LeftSmallBrace),
                (Token::StringValue(3)),
                (Token::RightSmallBrace),
                (Token::ID(6)),
                (Token::Store),
                (Token::ID(0)),
                (Token::LeftSmallBrace),
                (Token::ID(7)),
                (Token::LeftSmallBrace),
                (Token::RightSmallBrace),
                (Token::RightSmallBrace),
                (Token::ID(5)),
                (Token::LeftSmallBrace),
                (Token::ID(6)),
                (Token::RightSmallBrace),
                (Token::RightBigBrace),
            ],
        );
    }

    #[test]
    fn test_id_lex() {
        gen_test_token_env!(r#"id fuck _fuck 天帝abc abc天帝 t1"#, t);
        check(
            &mut t,
            vec![
                (Token::ID(0)),
                (Token::ID(1)),
                (Token::ID(2)),
                (Token::ID(3)),
                (Token::ID(4)),
                (Token::ID(5)),
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
                (Token::IntValue(convert_int_constval_to_usize(1))),
                (Token::IntValue(convert_int_constval_to_usize(23))),
            ],
        );
    }

    #[test]
    fn test_wrong_number2() {
        gen_test_token_env!(r#"0xabchds"#, t);
        check(
            &mut t,
            vec![
                (Token::IntValue(convert_int_constval_to_usize(0xabc))),
                (Token::ID(0)),
            ],
        );
        check_pool(vec!["hds".to_string()], &t.const_pool.name_pool);
    }

    #[test]
    fn test_next_back() {
        gen_test_token_env!(r#":()"#, t);
        let tmp = t.next_token().unwrap();
        assert_eq!(tmp, Token::Colon);
        t.next_back(tmp);
        assert_eq!(t.next_token().unwrap(), Token::Colon);
        check(
            &mut t,
            vec![(Token::LeftSmallBrace), (Token::RightSmallBrace)],
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
            vec![(Token::BoolValue(1)), (Token::ID(0)), (Token::BoolValue(0))],
        )
    }

    #[test]
    #[should_panic]
    fn test_braces_check2() {
        gen_test_token_env!(r#":)|"#, t);
        check(
            &mut t,
            vec![(Token::Colon), (Token::LeftSmallBrace), (Token::BitAnd)],
        );
    }

    #[test]
    #[should_panic]
    fn test_braces_check1() {
        gen_test_token_env!(r#":("#, t);
        check(&mut t, vec![(Token::Colon), (Token::LeftSmallBrace)]);
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
