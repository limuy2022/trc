/**
 * @file Compiler.h
 * @brief  编译器开放的接口
 * @date 2022-04-30
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <Compiler/compile_env.hpp>
#include <Compiler/compiler_def.hpp>
#include <Compiler/token.hpp>
#include <TVM/TVM.hpp>
#include <TVM/TVM_data.hpp>
#include <base/Error.hpp>
#include <language/error.hpp>
#include <limits>

namespace trc::compiler {
inline const char* main_module = "__main__";

using TVM_space::bytecode_t;
using TVM_space::byteCodeNumber;

/**
 * @brief 编译器的细节
 */
class Compiler {
public:
    Compiler(const std::string& module_name, const compiler_option& option,
        TVM_space::TVM_static_data& vm);

    /**
     * @brief 编译代码
     */
    void compile(const std::string& codes);

    // 编译期间需要用到的数据
    compiler_public_data compiler_data;

    // 编译时信息记录
    module_compile_env infoenv;

private:
    /**
     * @brief 添加字节码，并完善对应环境
     * @param opcode 字节码
     * @param index 字节码参数
     * @param bytecode 储存字节码的地方
     * @param line 当前行号
     */
    void add_opcode(byteCodeNumber opcode, TVM_space::bytecode_index_t index);

    /**
     * @brief 添加带有语句块的特殊代码
     */
    template <bool compiletype> void add_block();

    /**
     * @brief 检查下一个期待的token标签
     * @param tick 标签
     * @return token* 返回该符合要求的token
     */
    bool match(token_ticks tick);

    /**
     * @brief 提前获取下一个token标签
     */
    token_ticks get_next_token_tick();

    /**
     * @brief 清理换行符
     * @return 换行符后的下一个token
     */
    token clear_enter();

    bool sentence();

    void argv();

    bool value();

    bool func_call();

    bool factor();

    /**
     * @brief 用于解析运算符表达式
     * 
     * @param first_call 是否为首次调用，用于消除左递归
     * @return true 
     * @return false 
     */
    bool expr(bool first_call = true);

    /**
     * @brief 第一级运算符表达式
     * 
     * @param first_call 是否为首次调用，用于消除左递归
     * @return true 
     * @return false 
     */
    bool term(bool first_call = true);

    /**
     * @brief 根据token制作相应的数据节点
     * @warning 不负责变量节点的生成
     */
    bool const_value();

    bool id(bool error_report);

    bool item(bool error_reoprt);

    token lookahead;

    /**
     * @brief 生成一条字节码的对应行号表记录
     */
    void generate_line_table(line_t line);

    line_t prev_value = std::numeric_limits<size_t>::max();

    // 行号转化为字节码每行的第一个字节码的索引
    std::vector<line_t> line_to_bycodeindex_table;

    // token解析器
    trc::compiler::token_lex token_;

    // 当前环境
    basic_compile_env* local {};
};

template <bool compiletype> void Compiler::add_block() {
    // 记录语句块一开始的位置
    size_t goto_addr;
    if constexpr (compiletype) {
        goto_addr = local->bytecode.size();
    }
    // 表达式
    item(true);
    // 单独处理跳转语句，否则无法处理常量表
    // 0只是用来占位的，后面会被修改为正确的地址
    add_opcode(byteCodeNumber::IF_FALSE_GOTO, 0);
    // 获取跳转表达式的字节码位置
    size_t fix_bytecode = local->bytecode.size() - 1;
    // 然后编译其它所有的节点
    while (get_next_token_tick() != token_ticks::RIGHT_BIG_BRACE) {
        if (!sentence()) {
            compiler_data.error.send_error_module(
                error::SyntaxError, language::error::syntaxerror);
        }
    }
    if constexpr (compiletype) {
        // while循环比if多一句goto
        add_opcode(byteCodeNumber::GOTO, goto_addr);
    }
    // 然后重新修改跳转地址
    local->bytecode[fix_bytecode].index = local->bytecode.size();
}
}
