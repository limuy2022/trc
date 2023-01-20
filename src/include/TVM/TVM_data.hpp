/**
 * TVM各种数据集合的定义处
 */

#pragma once

#include <TVM/TVMdef.hpp>
#include <TVM/func.hpp>
#include <TVM/object.hpp>
#include <string>
#include <vector>

namespace trc::TVM_space {
class TVM_bytecode {
public:
    TVM_bytecode(byteCodeNumber bycode, bytecode_index_t index)
        : bycode(bycode)
        , index(index) {
    }
    byteCodeNumber bycode;
    bytecode_index_t index;
};

/**
 * @brief 静态数据
 * 本类用于分离TVM的职责，装载编译时的数据，如常量池等
 * 并且降低TVM和Compiler之间的耦合度
 */
class TVM_static_data {
public:
    // 各种常量数据
    std::vector<int> const_i;
    std::vector<std::string> const_s, const_long;
    std::vector<double> const_f;
    std::vector<func_> funcs;
    // 字节码
    struct_codes byte_codes;
    // 类信息
    std::vector<class_header> class_msg;
    // 行号表，字节码对应到相应行号的表(可以选择不生成)
    std::vector<line_t> line_number_table;
    // 全局符号表大小
    size_t global_symbol_table_size {};
    void compress_memory();
};

constexpr size_t TVM_STACK_SIZE = 8192;
constexpr size_t TVM_FRAME_SIZE = 1024;

/**
 * @brief TVM运行过程中动态数据的存放地
 * 派生类：例如：TVM， frame_
 */
class TVM_dyna_data {
public:
    // 操作数栈
    def::OBJ stack_data[TVM_STACK_SIZE] {};
    // 栈顶指针
    def::OBJ* stack_top_ptr;
    // 帧栈顶指针
    frame_* frames_top_str;
    // 帧栈
    frame_ frames[TVM_FRAME_SIZE];
    // 全局符号表
    symbol_form global_symbol_table;

    TVM_dyna_data();
    void reset_global_symbol_table(size_t size);
};
}
