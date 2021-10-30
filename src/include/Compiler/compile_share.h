#ifndef TRC_INCLUDE_COMPILER_COMPILE_SHARE_H
#define TRC_INCLUDE_COMPILER_COMPILE_SHARE_H

#include <cctype>
#include "share.h"

using namespace std;

// 宏，判断是否为数字，包括-和.
#define is_num(c) (check_in((c), num))
// 判断是否为字母，包括_
#define is_english(c) (isalpha((c)) || (c) == '_')
// 判断是否为数字或字符，不包含.和-，包含_
#define is_english_num(c) (isdigit((c)) || is_english((c)))

// 以整型作为标识符
// 纯粹数据
#define DATA 1
// 生成无参数字节码
#define OPCODE 2
// 生成无法通过符号转化的字节码
#define ORIGIN 3
// 内置函数
#define BUILTIN_FUNC 4
// 树
#define TREE 5
// 带有参数的字节码
#define OPCODE_ARGV 6
// 函数定义，由于字节码十分特殊
#define FUNC_DEFINE 7
// 变量定义，由于变量作用域特殊
#define VAR_DEFINE 8
// 调用自定义函数
#define CALL_FUNC 9

extern const vecs sentences;
extern const vecs sentences_yes_argv;
extern const vecs keywords;
extern const vecs str;
extern const vecs opti_opers;
extern const vecs opti_condits;
extern const vecs condits;
extern const vecs highest_condits;
extern const vecs other_condits;
extern const vecs aslist;
extern map<char, string> change_varchar;
extern const vecs opers;
extern const vecs in_func_char;
extern const vecs const_values;
extern map<string, int> change_const;

#endif
