/**
 * @file Compiler.h
 * @brief  编译器开放的接口
 * @date 2022-04-30
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <Compiler/library.h>
#include <Compiler/pri_compiler.hpp>
#include <TVM/TVM.h>
#include <base/Error.h>
#include <base/trcdef.h>
#include <stack>

namespace trc::compiler {
class grammar_data_control;

typedef error::error_module compiler_error;

/**
 * @brief
 * 这是一个将字符串转换成token流按行输出的类
 */
class TRC_Compiler_api token_lex {
public:
    token_lex(const std::string& code, compiler_error* error_);

    ~token_lex();

    /**
     * @brief
     * 从当前字符串代码中读取一个token并返回给grammar解析
     *
     * @return token* 返回一个有意义的token
     * @return
     * 特殊返回值：由于总是保证返回有意义，所以当token的tick为token_ticks::END_OF_TOKENS时，表示解析结束
     */
    token* get_token();

    void unget_token(token* token_data);

    compiler_error* error_;

private:
    bool is_unget = false;

    token* back_token = nullptr;

    // 经过简单处理的原始字符串代码
    const std::string& rawcode;

    // 指向当前正在解析的字符
    const char* char_ptr;

    // 获取下一个字符
    void get_next_char() noexcept;

    // 回到上一个字符处
    void to_back_char() noexcept;

    // 判断是否解析到了终点
    bool end_of_lex() const noexcept;

    // 解析数字（包括浮点数）
    void lex_int_float(token* result);

    /**
     * @brief 解析一个字符串
     * 注：会略过开头结尾的"和'符号
     */
    void lex_string(token* result);

    /**
     * @brief
     * 解析英文符号（包含关键字和名称两种可能）
     */
    void lex_english(token* result);

    /**
     * @brief 解析其他字符，如[],()等
     * @details
     * 在这里解析的字符都能被用token_ticks完整表达，所以不需要储存任何信息
     */
    void lex_others(token* result);

    /**
     * 解析符号时遇到多种情况，例如读取*后可以为*，*=，**，**=四种情况
     * 使用方法：传入符号后期待的符号，如*期待=,再依次传入期待满足时的标记和期待不满足时的标记
     */
    token_ticks get_binary_ticks(
        char expected_char, token_ticks expected, token_ticks unexpected);

    /**
     * 只有在有且仅有一个期待字符时使用
     * @brief
     * 检查下一个字符是不是期待的字符，如果接下来是不是期待的字符，直接报错
     *
     * @param expected_char 接下来唯一期待的字符
     */
    void check_expected_char(char expected_char);

    // 用于检查代码是否正确
    std::stack<char> check_brace;
};

/**
 * @brief
 * 传入一台虚拟机和未编译的代码，将代码编译后填充进vm中
 * @param vm 可以是编译过的虚拟机
 * @param codes 源代码
 * @warning 使用前需要提前用TVM_space::free_TVM去释放内存
 */
TRC_Compiler_func_api void Compiler(
    TVM_space::TVM* vm, const std::string& codes);

/**
 * @brief 释放整棵树的内存
 */
TRC_Compiler_func_api void free_tree(compiler::treenode* head);
}
