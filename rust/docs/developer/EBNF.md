# the ebnf of the trc

help to develop the compiler

programs : statements
statements : statements statement
statement :
    ID := expr |
    expr |
    ID = expr
item : ID | int | float | string
opt_argvs: argvs | empty
argvs : argvs , argv | argv
argv : expr

expr : expr1 expr_
expr_ : || expr1 expr_ | empty

expr1 : expr2 expr1_
expr1_ : && expr2 expr1_ | empty  

expr2 : ! expr3 | expr3

expr3 : expr4 expr3_
expr3_ : == expr3_| != expr3_ | < expr3_| > expr3_ | <= expr3_| >= expr3_ |empty

expr4 : expr5 expr4_
expr4_: | expr4_ | empty

expr5 : expr6 expr5_
expr5_: ^ expr5_ | empty

expr6 : expr7 expr6_
expr6_: & expr6_ | empty

expr7: expr8 expr7_
expr7_: << expr7_| >> expr7_ | empty

expr8 : expr9 expr8_
expr8_: + expr8_| - expr8_ | empty

expr9 : expr10 expr9_
expr9_: * expr9_| / expr9_ | % expr9_| // expr9_ | empty

expr10: - expr11| + expr11 | ~expr |expr11

expr11: factor expr11_
expr11_: ** expr11_ | empty
factor : (expr) | item

expr : ID(opt_argvs)
