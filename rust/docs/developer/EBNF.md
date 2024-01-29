# the ebnf of the trc

help to develop the compiler

programs : statements
statements : statements statement
statement :
    ID := expr
    ID(argvs)
item : ID | int | float | string
opt_argvs: argvs | empty
argvs : argvs , argv | argv
argv : expr
expr : expr + term | expr - term
term : term * factor | term / factor
factor : (expr) | item
