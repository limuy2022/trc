/**
 * @file Compiler.h
 * @brief  编译器开放的接口
 * @date 2022-04-30
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <Compiler/compile_env.h>
#include <Compiler/compiler_def.h>
#include <Compiler/library.h>
#include <Compiler/pri_compiler.hpp>
#include <TVM/TVM.h>
#include <base/Error.h>
#include <base/trcdef.h>

namespace trc::compiler {
class grammar_data_control;

/**
 * @brief 编译器的细节
 */
class TRC_Compiler_api detail_compiler {
public:
    detail_compiler(compiler_public_data&, TVM_space::TVM* vm);

    /**
     * @brief 将一个节点解析成字节码
     * @param head 根节点
     */
    void compile(treenode* head);
    // 编译时信息记录
    CompileEnvironment infoenv;

    /**
     * @brief 删除多余的信息
     * @details
     * 为何不在析构函数中解决该问题？因为当该类作为外传的信息类时，就要承担起释放多余内存的责任
     */
    void free_detail_compiler();

    /**
     * @brief 重新绑定数据
     */
    void retie(TVM_space::TVM* vm);

    // 编译期间需要用到的数据
    compiler_public_data& compiler_data;

private:
    /**
     * @brief 将整形数据添加进const_i常量池
     * @return 数据在常量池中的索引
     */
    TVM_space::bytecode_index_t add_int(int value);

    /**
     * @brief 将浮点型数据添加进const_f常量池
     * @return 数据在常量池中的索引
     */
    TVM_space::bytecode_index_t add_float(float value);

    /**
     * @brief 将字符串型数据添加进const_s常量池
     * @return 数据在常量池中的索引
     */
    TVM_space::bytecode_index_t add_string(const char* value);

    /**
     * @brief 将变量添加进符号表中
     * @return 变量在符号表中的索引
     * @warning 不允许变量不存在于符号表中
     */
    TVM_space::bytecode_index_t add_var_allow_not_in(const char* value);

    /**
     * @brief 将变量添加进符号表中
     * @return 变量在符号表中的索引
     * @warning 允许变量不存在于符号表中
     */
    TVM_space::bytecode_index_t add_var_must_in(const char* value);

    /**
     * @brief 将长整型添加进符号表中
     * @return 变量在符号表中的索引
     */
    TVM_space::bytecode_index_t add_long(const char* value);

    /**
     * @brief
     * 构建字节码，以data在opcodesym_int中的值和index初始化数组
     * @param data 符号
     * @param index 索引
     */
    static TVM_space::bytecode_t build_opcode(token_ticks symbol);

    /**
     * @brief 构建有关变量的字节码
     * @param data 符号
     * @param index 索引
     */
    static TVM_space::bytecode_t build_var(token_ticks data);

    /**
     * @brief 添加字节码，并完善对应环境
     */
    void add_opcode(TVM_space::bytecode_t, TVM_space::bytecode_index_t index);

    /**
     * @brief 解析函数字节码
     */
    void func_lexer(treenode* head);

    long long prev_value = LLONG_MAX;

    // 行号转化为字节码每行的第一个字节码的索引
    std::vector<int> line_to_bycodeindex_table;

    TVM_space::TVM* vm;
};

/**
 * @brief
 * 传入一台虚拟机和未编译的代码，将代码编译后填充进vm中
 * @param vm 虚拟机,编译过的要用TVM_space::free_TVM去释放内存
 * @param codes 源代码
 * @param option 编译器参数,如果已经指定编译器传入nullptr
 * @param compiler_ptr
 * 提供的编译器，不指定则自己创建，指定则可以使用上一次编译的信息
 * @param return_compiler_ptr
 * 是否返回当前编译函数生成的编译器对象，可用于保存编译信息
 * @warning 使用前需要提前用TVM_space::free_TVM去释放内存
 */
TRC_Compiler_c_api detail_compiler* Compiler(TVM_space::TVM* vm,
    const std::string& codes, const compiler_option* option,
    detail_compiler* compiler_ptr = nullptr, bool return_compiler_ptr = false);

/**
 * @brief 释放整棵树的内存
 */
TRC_Compiler_c_api void free_tree(compiler::treenode* head);
}
