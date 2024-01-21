use super::TokenLex;

pub struct AstBuilder<'a> {
    token_lexer: TokenLex<'a>,
}

impl<'a> AstBuilder<'a> {
    pub fn new(token_lexer: TokenLex<'a>) -> Self {
        AstBuilder { token_lexer }
    }
}
