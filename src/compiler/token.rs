use super::{Context, ValuePool};
use crate::compiler::CompilerImpl;
use libcore::*;
use logos::{Lexer, Logos, Source};
use rust_i18n::t;
use std::{cell::RefCell, fmt::Display, rc::Rc};

fn add_float(lex: &mut Lexer<Token>) -> ConstPoolData {
    // println!("{}", lex.slice());
    lex.extras.add_float(lex.slice().to_owned())
}

pub const CONST_IDX_PLACEHOLDER: ConstPoolData = ConstPoolData(0);

fn add_string(lex: &mut Lexer<Token>) -> ConstPoolData {
    // 此处还需要进行字符串
    let origin_str = lex.slice();
    let mut target_string = String::new();
    target_string.reserve(origin_str.len());
    let mut iter = origin_str.chars();
    iter.next();
    if lex.slice().starts_with(r#"""""#)
        && lex.slice().ends_with(r#"""""#)
        && lex.slice().len() >= 6
    {
        iter.next();
        iter.next();
    }
    while let Some(i) = iter.next() {
        if i == '\\' {
            if let Some(j) = iter.next() {
                match j {
                    'n' => target_string.push('\n'),
                    't' => target_string.push('\t'),
                    'r' => target_string.push('\r'),
                    '0' => target_string.push('\0'),
                    '"' => target_string.push('"'),
                    '\'' => target_string.push('\''),
                    _ => {
                        target_string.push('\\');
                        target_string.push(j)
                    }
                }
            } else {
                panic!("\\ is the end of the string but the compiler is not find it out.")
            }
        } else {
            target_string.push(i);
        }
    }
    target_string.pop();
    lex.extras.add_string(target_string)
}

fn add_id(lex: &mut Lexer<Token>) -> ConstPoolData {
    lex.extras.add_id(lex.slice().to_owned())
}

fn lex_muli_comment(lex: &mut Lexer<Token>) -> Result<usize, ErrorInfo> {
    let source = lex.remainder();
    let mut n = 0;
    let mut lines = 0;
    loop {
        match source.slice(n..(n + 1)) {
            None => return Err(ErrorInfo::new(t!(UNCLODED_COMMENT), t!(SYNTAX_ERROR))),
            Some(v) => {
                let c = v.chars().next().unwrap();
                if c == '\n' {
                    lines += 1;
                } else if c == '*' && source.slice((n + 1)..(n + 2)) == Some("/") {
                    n += 2;
                    break;
                }
                n += 1;
            }
        }
    }
    lex.bump(n);
    Ok(lines)
}

fn convert_int(lex: &mut Lexer<Token>) -> ConstPoolData {
    // println!("{}", lex.slice());
    let str_tmp = lex.slice().replace("_", "");
    ConstPoolData(if let Some(s) = str_tmp.strip_prefix("0x") {
        let val: i64 = i64::from_str_radix(s, 16).unwrap();
        convert_int_constval_to_usize(val)
    } else if let Some(s) = str_tmp.strip_prefix("0b") {
        let val: i64 = i64::from_str_radix(s, 2).unwrap();
        convert_int_constval_to_usize(val)
    } else if let Some(s) = str_tmp.strip_prefix("0o") {
        let val: i64 = i64::from_str_radix(s, 8).unwrap();
        convert_int_constval_to_usize(val)
    } else {
        let val: i64 = str_tmp.parse().unwrap();
        convert_int_constval_to_usize(val)
    })
}

fn convert_char(lex: &mut Lexer<Token>) -> Result<ConstPoolData, ErrorInfo> {
    let len = lex.slice().len();
    if len != 3 {
        return Err(ErrorInfo::new(t!(CHAR_FORMAT), t!(SYNTAX_ERROR)));
    }
    let mut iter = lex.slice().chars();
    iter.next();
    Ok(ConstPoolData(iter.next().unwrap() as usize))
}

#[derive(PartialEq, Debug, Clone, Hash, Eq, Copy, Logos)]
#[logos(extras = ValuePool)]
#[logos(skip r"[ \t\r\f]+")]
#[logos(error = ErrorInfo)]
pub enum Token {
    // #[regex(r"/\*[^(\*/)\n]*", logos::skip)]
    #[regex(r"/\*", lex_muli_comment)]
    CrossLinesComment(usize),
    #[regex(r"\n")]
    NewLine,
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
    #[regex(
        r#"(0[bB][01_]+|0[oO][0-7_]+|0[xX][0-9a-fA-F_]+|[0-9][0-9_]*)"#,
        convert_int
    )]
    IntValue(ConstPoolData),
    #[regex(r#""[^"]*""#, add_string)]
    // #[regex(r#""""(.)*""""#, add_string)]
    // TODO:MULT
    StringValue(ConstPoolData),
    #[regex(r#"((\d+)\.(\d+))|(((\d+)|(\d+\.\d+))[eE][-+]?\d+)?"#, add_float)]
    FloatValue(ConstPoolData),
    LongIntValue,
    #[regex(r#"'[^']*'"#, convert_char)]
    CharValue(ConstPoolData),
    #[token("false", |_| ConstPoolData(false as usize))]
    #[token("true", |_| ConstPoolData(true as usize))]
    BoolValue(ConstPoolData),
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
    #[regex(r#"[\p{XID_Start}_]\p{XID_Continue}*"#, add_id)]
    ID(ConstPoolData),
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
    #[regex(r"#.*", logos::skip)]
    Comment,
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

    fn to_brace(self) -> char {
        match self {
            Token::LeftBigBrace => '{',
            Token::RightBigBrace => '}',
            Token::LeftMiddleBrace => '[',
            Token::RightMiddleBrace => ']',
            Token::LeftSmallBrace => '(',
            Token::RightSmallBrace => ')',
            _ => unreachable!(),
        }
    }
}

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
            Token::NewLine => r#"\n"#,
            Token::Comment => "Comment",
            Token::CrossLinesComment(_) => "CrossLinesComment",
        };
        write!(f, "{res}")
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

pub struct TokenLex<'a> {
    compiler_data: Rc<RefCell<CompilerImpl>>,
    braces_check: Vec<BraceRecord>,
    // token和当前行号
    unget_token: Vec<(Token, usize)>,
    internal_lexer: Lexer<'a, Token>,
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

impl Iterator for TokenLex<'_> {
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

impl<'a> TokenLex<'a> {
    pub fn new(compiler_data: Rc<RefCell<CompilerImpl>>) -> TokenLex<'a> {
        let internal_lexer = Token::lexer_with_extras(
            unsafe { &(*(compiler_data.borrow().get_source() as *const str)) },
            ValuePool::default(),
        );
        TokenLex {
            compiler_data,
            braces_check: Vec::new(),
            unget_token: Vec::new(),
            internal_lexer,
        }
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

    pub fn modify_input(&mut self, source: String) {
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
        loop {
            let token = self.internal_lexer.next();
            return match token {
                Some(t) => {
                    let t = match t {
                        Ok(data) => data,
                        Err(e) => {
                            return self.report_error(e);
                        }
                    };
                    if t == Token::LeftBigBrace
                        || t == Token::LeftSmallBrace
                        || t == Token::LeftMiddleBrace
                    {
                        self.braces_check.push(BraceRecord::new(
                            t.to_brace(),
                            self.compiler_data.borrow().context.get_line(),
                        ));
                    } else if t == Token::RightBigBrace
                        || t == Token::RightSmallBrace
                        || t == Token::RightMiddleBrace
                    {
                        self.check_braces_stack(t.to_brace())?;
                    } else if t == Token::NewLine {
                        // println!("fuck you ccf");
                        self.add_line();
                        continue;
                    } else if let Token::CrossLinesComment(v) = t {
                        self.compiler_data.borrow_mut().context.add_line_by(v);
                        continue;
                    }
                    Ok(t)
                }
                None => Ok(Token::EndOfFile),
            };
        }
    }

    pub fn add_line(&mut self) {
        self.compiler_data.borrow_mut().context.add_line();
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

    pub fn add_id(&mut self, id: String) -> ConstPoolData {
        self.internal_lexer.extras.add_id(id)
    }

    pub fn get_constpool(&self) -> &ValuePool {
        &self.internal_lexer.extras
    }

    pub fn get_constpool_mut(&mut self) -> &mut ValuePool {
        &mut self.internal_lexer.extras
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

    /// check token
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
                "{i} not in pool.{testpool:?} is expected pool\n{pool_be_checked:?} is checked pool"
            );
        }
    }

    /// 不关心具体的token值,一直解析到文件末尾，可用于检查报错是否正常
    fn check_until_eof(tokenlex: &mut TokenLex) {
        loop {
            let token_tmp = tokenlex.next_token().unwrap();
            println!("{token_tmp:?}");
            if token_tmp == Token::EndOfFile {
                break;
            }
        }
        // println!("fuck you ccf");
        tokenlex.check().unwrap();
    }

    #[test]
    fn test_comment() {
        gen_test_token_env!(
            r"

        /**/
# comment
                # comment
                #
/* comment */
        /*
         *dwdwdwd
         *dwdw
         */
        ",
            t
        );

        check(&mut t, vec![])
    }

    #[test]
    fn test_numberlex() {
        gen_test_token_env!(
            r#",,.,100
        
        
        123.9 232_304904
 0b011
0x2aA4
0o2434   0 0   1e9  1.2e1 8e-1 18E-4 1.7e-2 1.98e2
 "#,
            t
        );
        check(
            &mut t,
            vec![
                Token::Comma,
                Token::Comma,
                Token::Dot,
                Token::Comma,
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(100))),
                Token::FloatValue(ConstPoolData(0)),
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(232_304904))),
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(0b011))),
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(0x2AA4))),
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(0o2434))),
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(0))),
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(0))),
                Token::FloatValue(ConstPoolData(1)),
                Token::FloatValue(ConstPoolData(2)),
                Token::FloatValue(ConstPoolData(3)),
                Token::FloatValue(ConstPoolData(4)),
                Token::FloatValue(ConstPoolData(5)),
                Token::FloatValue(ConstPoolData(6)),
            ],
        );
        check_pool(
            vec!["123.9", "1.2e1", "8e-1", "18E-4", "1.7e-2", "1.98e2", "1e9"],
            &t.internal_lexer.extras.const_floats,
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
                Token::Colon,
                Token::LeftBigBrace,
                Token::RightBigBrace,
                Token::LeftMiddleBrace,
                Token::RightMiddleBrace,
                Token::LeftSmallBrace,
                Token::RightSmallBrace,
                Token::SelfAdd,
                Token::SelfMod,
                Token::SelfExactDiv,
                Token::ExactDiv,
                Token::SelfDiv,
                Token::Power,
                Token::SelfMul,
                Token::Mul,
                Token::Comma,
                Token::BitRightShift,
                Token::BitLeftShift,
                Token::SelfBitRightShift,
                Token::Or,
                Token::And,
                Token::DoubleColon,
                Token::Store,
                Token::Colon,
            ],
        );
    }

    #[test]
    fn test_string_lex() {
        gen_test_token_env!(r#""s"'s'"sdscdcdfvf""depkd""""\n\t""ttt\tt""#, t);
        check(
            &mut t,
            vec![
                Token::StringValue(ConstPoolData(0)),
                Token::CharValue(ConstPoolData('s' as usize)),
                Token::StringValue(ConstPoolData(1)),
                Token::StringValue(ConstPoolData(2)),
                Token::StringValue(ConstPoolData(3)),
                Token::StringValue(ConstPoolData(4)),
                Token::StringValue(ConstPoolData(5)),
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
            &t.internal_lexer.extras.const_strings,
        );
    }

    #[test]
    fn test_complex_lex() {
        gen_test_token_env!(
            r#"
            
        /**/
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
                Token::Import,
                Token::StringValue(ConstPoolData(0)),
                Token::Func,
                Token::ID(ConstPoolData(0)),
                Token::LeftSmallBrace,
                Token::ID(ConstPoolData(1)),
                Token::ID(ConstPoolData(2)),
                Token::RightSmallBrace,
                Token::Arrow,
                Token::ID(ConstPoolData(3)),
                Token::LeftBigBrace,
                Token::If,
                Token::ID(ConstPoolData(2)),
                Token::Mod,
                Token::IntValue(ConstPoolData(2)),
                Token::Equal,
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(0))),
                Token::LeftBigBrace,
                Token::Return,
                Token::StringValue(ConstPoolData(1)),
                Token::RightBigBrace,
                Token::Else,
                Token::LeftBigBrace,
                Token::Return,
                Token::StringValue(ConstPoolData(2)),
                Token::RightBigBrace,
                Token::RightBigBrace,
                Token::Func,
                Token::ID(ConstPoolData(4)),
                Token::LeftSmallBrace,
                Token::RightSmallBrace,
                Token::LeftBigBrace,
                Token::ID(ConstPoolData(5)),
                Token::LeftSmallBrace,
                Token::StringValue(ConstPoolData(3)),
                Token::RightSmallBrace,
                Token::ID(ConstPoolData(6)),
                Token::Store,
                Token::ID(ConstPoolData(0)),
                Token::LeftSmallBrace,
                Token::ID(ConstPoolData(7)),
                Token::LeftSmallBrace,
                Token::RightSmallBrace,
                Token::RightSmallBrace,
                Token::ID(ConstPoolData(5)),
                Token::LeftSmallBrace,
                Token::ID(ConstPoolData(6)),
                Token::RightSmallBrace,
                Token::RightBigBrace,
            ],
        );
    }

    #[test]
    fn test_id_lex() {
        gen_test_token_env!(r#"id fuck _fuck 天帝abc abc天帝 t1"#, t);
        check(
            &mut t,
            vec![
                Token::ID(ConstPoolData(0)),
                Token::ID(ConstPoolData(1)),
                Token::ID(ConstPoolData(2)),
                Token::ID(ConstPoolData(3)),
                Token::ID(ConstPoolData(4)),
                Token::ID(ConstPoolData(5)),
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
            &t.internal_lexer.extras.name_pool,
        );
    }

    #[test]
    fn test_wrong_number1() {
        gen_test_token_env!(r#"0b123"#, t);
        check(
            &mut t,
            vec![
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(1))),
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(23))),
            ],
        );
    }

    #[test]
    fn test_wrong_number2() {
        gen_test_token_env!(r#"0xabchds"#, t);
        check(
            &mut t,
            vec![
                Token::IntValue(ConstPoolData(convert_int_constval_to_usize(0xabc))),
                Token::ID(ConstPoolData(0)),
            ],
        );
        check_pool(vec!["hds".to_string()], &t.internal_lexer.extras.name_pool);
    }

    #[test]
    fn test_next_back() {
        gen_test_token_env!(r#":()"#, t);
        let tmp = t.next_token().unwrap();
        assert_eq!(tmp, Token::Colon);
        t.next_back(tmp);
        assert_eq!(t.next_token().unwrap(), Token::Colon);
        check(&mut t, vec![Token::LeftSmallBrace, Token::RightSmallBrace]);
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
    #[should_panic(expected = "cannot")]
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
                Token::BoolValue(ConstPoolData(1)),
                Token::ID(ConstPoolData(0)),
                Token::BoolValue(ConstPoolData(0)),
            ],
        )
    }

    #[test]
    #[should_panic]
    fn test_braces_check2() {
        gen_test_token_env!(r#":)|"#, t);
        check(
            &mut t,
            vec![Token::Colon, Token::LeftSmallBrace, Token::BitAnd],
        );
    }

    #[test]
    #[should_panic]
    fn test_braces_check1() {
        gen_test_token_env!(r#":("#, t);
        check(&mut t, vec![Token::Colon, Token::LeftSmallBrace]);
    }

    #[test]
    #[should_panic(expected = "char")]
    fn test_error_char() {
        gen_test_token_env!(r#"print("{}", 'pp')"#, t);
        check_until_eof(&mut t);
    }

    #[test]
    #[should_panic(expected = "cannot")]
    fn test_error_str() {
        gen_test_token_env!(r#"print("{}", "pp)"#, t);
        check_until_eof(&mut t);
    }
}
