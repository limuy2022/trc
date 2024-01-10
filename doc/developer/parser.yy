%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.2"
%header

/* %define api.token.raw */

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  # include <string>
  namespace trc::compiler {
    class compiler;
  }
}

// The parsing context.
%param { trc::compiler::compiler& drv }

%locations

%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {
# include "compiler.hpp"
}

/* %define api.value.type variant */
%token FOR // for
    WHILE // while
    IF // if
    FUNC // function
    CLASS // class
    ADD // +
    SUB // -
    MUL // *
    DIV // /
    ZDIV // //
    MOD // %
    POW // **
    AND // and
    OR // or
    NOT // not
    EQUAL // ==
    UNEQUAL // !=
    LESS // <
    GREATER // >
    LESS_EQUAL // <=
    GREATER_EQUAL // >=
    IMPORT // import
    GOTO // goto
    DEL // del
    ASSERT // assert
    BREAK // break
    CONTINUE // continue
    SELFADD // +=
    SELFSUB // -=
    SELFMUL // *=
    SELFDIV // /=
    SELFZDIV // //=
    SELFMOD // %=
    SELFPOW // **=
    ASSIGN // =
    STORE // :=
    NAME // 名称
    NULL_ // null
    TRUE_ // true
    FALSE_ // false
    STRING_VALUE // 字符串值
    LONG_FLOAT_VALUE // 长浮点型值
    FLOAT_VALUE // 浮点数值
    LONG_INT_VALUE // 长整型值
    INT_VALUE // 整型值
    LEFT_BIG_BRACE // {
    RIGHT_BIG_BRACE // }
    LEFT_SMALL_BRACE // (
    RIGHT_SMALL_BRACE // )
    LEFT_MID_BRACE // [
    RIGHT_MID_BRACE // ]
    POINT //.
    COMMA // ,
    ELSE
    IN
    RETURN
    PUBLIC
    PRIVATE
%start program
%%
program : statements
statements : statements statement | statement
statement :
            ifs |
            whiles |
            fors |
            funcdef |
            sentence |
            classdef
expr : 
            expr ADD term |
            expr SUB term |
            term |
            SUB expr |
            expr IN expr
lval : 
            expr LEFT_MID_BRACE expr RIGHT_MID_BRACE |
            expr POINT expr
item :
            expr |
            NAME |
            lval
term :
            term MUL termdeep |
            term DIV termdeep |
            term MOD termdeep |
            termdeep ZDIV factor
termdeep :
            termdeep POW termdeep2
termdeep2 :
            termdeep2 GREATER termdeep3 |
            termdeep2 GREATER_EQUAL termdeep3 |
            termdeep2 LESS termdeep3 |
            termdeep2 LESS_EQUAL termdeep3 |
            termdeep2 UNEQUAL termdeep3 |
            termdeep2 EQUAL termdeep3
termdeep3 : termdeep3 OR termdeep4
termdeep4 : termdeep4 AND factor
num :
            INT_VALUE |
            FLOAT_VALUE |
            LONG_INT_VALUE |
            LONG_FLOAT_VALUE
factor : 
            NAME |
            num |
            LEFT_SMALL_BRACE expr RIGHT_SMALL_BRACE |
            NOT factor
ifs : 
            IF expr LEFT_BIG_BRACE statements RIGHT_BIG_BRACE |
            IF expr LEFT_BIG_BRACE statements RIGHT_BIG_BRACE ELSE LEFT_BIG_BRACE statements RIGHT_BIG_BRACE
whiles : 
            WHILE expr LEFT_BIG_BRACE statements RIGHT_BIG_BRACE |
            WHILE expr LEFT_BIG_BRACE statements RIGHT_BIG_BRACE ELSE LEFT_BIG_BRACE statements RIGHT_BIG_BRACE
fors : FOR NAME IN item LEFT_BIG_BRACE statements RIGHT_BIG_BRACE ELSE LEFT_BIG_BRACE statements RIGHT_BIG_BRACE 
args : args COMMA NAME
optargs : args | %empty
funcdef : FUNC NAME LEFT_SMALL_BRACE optargs RIGHT_SMALL_BRACE LEFT_BIG_BRACE statements RIGHT_BIG_BRACE
valdef : lval STORE expr
optval : expr | %empty
sentence :
            NAME LEFT_SMALL_BRACE optargs RIGHT_SMALL_BRACE |
            GOTO NAME |
            RETURN optval |
            BREAK |
            CONTINUE |
            lval ASSIGN expr |
            valdef |
            lval SELFADD expr |
            lval SELFDIV expr |
            lval SELFMOD expr |
            lval SELFMUL expr |
            lval SELFPOW expr |
            lval SELFSUB expr |
            lval SELFZDIV expr |
            IMPORT STRING_VALUE
funcdef_valdef : funcdef_valdef funcdef | funcdef_valdef valdef
opt_funcdef_valdef : funcdef_valdef | %empty
opt_pub : PUBLIC | %empty
opt_pri : PRIVATE | %empty
classdef :
            CLASS NAME LEFT_SMALL_BRACE NAME RIGHT_SMALL_BRACE LEFT_BIG_BRACE opt_funcdef_valdef RIGHT_BIG_BRACE
assert : ASSERT expr
del : DEL expr
%%
