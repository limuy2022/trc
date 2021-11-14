/**
 * 编译时期共享的数据
 */

#include <string>
#include <map>
#include "Compiler/compile_share.h"

using namespace std;

// 语句
const vecs sentences = {"import", "goto", "del", "assert"};
// 带有参数的语句（满足有且只有一个的语句）
const vecs sentences_yes_argv = {"goto"};
// 关键字
const vecs keywords = {"if", "while", "for", "func", "class"};
// 条件运算符
// 最高级运算符
const vecs highest_condits = {"and", "or"};
// 除了最高级运算符以外的运算符
const vecs other_condits = {"<", ">", "<=", ">=", "!=", "=="};
// 所有运算符
const vecs condits = {"<", ">", "<=", ">=", "!=", "==", "and", "or", "not"};

// 等号
const vecs aslist = {":=", "="};
// 运算符
const vecs opers = {
        "+", "-", "**", "*",
        "/", "//", "%", "+=",
        "-=", "*=", "/=", "**=",
        "%=", "//=",
};

// 可以优化的运算符
const vecs opti_opers = {
        "+", "-", "**",
        "*", "//", "%",
};

// 可以优化的条件运算符
const vecs opti_condits = {
        "<", ">", "<=", ">=",
        "!=", "==", "and", "or"
};

// 函数有时与其它运算符优先级关系并不确定，需要通过括号判定，in_func_char包括了这些字符
const vecs in_func_char = {
        "+", "-", "**", "*", "/", "//",
        "%", "<", ">", "<=", ">=", "!=",
        "==", "and", "or"
};

// 转义字符对应表
map<char, string> change_varchar = {{'n',  "\n"},
                                    {'t',  "\t"},
                                    {'\'', "'"},
                                    {'"',  R"(")"},
                                    {'\\', R"(\)"}};
// 常量值
const vecs const_values = {
        "true",
        "false",
        "null"
};

// 常量值到数字的转换

map<string, int> change_const = {
        {"true",  1},
        {"false", 0},
        {"null",  0},
};
