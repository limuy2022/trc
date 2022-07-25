/**
 * TVM各种数据集合的定义处
 */

#pragma once

#include <TVM/TVMdef.h>
#include <TVM/library.h>
#include <TVM/object.h>
#include <map>
#include <stack>
#include <string>
#include <vector>

namespace trc::TVM_space {
class func_;

class frame_;

class TVM_bytecode {
public:
    TVM_bytecode(bytecode_t bycode, bytecode_index_t index)
        : bycode(bycode)
        , index(index) {
    }
    bytecode_t bycode;
    bytecode_index_t index;
};

/**
 * @brief 符号表，保存变量信息
 * 就是凭借这个结构优化变量访问速度为O(1)
 */
class symbol_form {
public:
    def::OBJ* vars = nullptr;
    void reset(size_t size);
    symbol_form(size_t size);
    symbol_form() = default;
    ~symbol_form();
};

/**
 * @brief 静态数据
 * 本类用于分离TVM的职责，装载编译时的数据，如常量池等
 * 并且降低TVM和Compiler之间的耦合度
 */
class TRC_TVM_api TVM_static_data {
public:
    // 各种常量数据
    std::vector<int> const_i;
    std::vector<char*> const_s, const_long;
    std::vector<double> const_f;
    // 注意：此处装载func的静态信息，并不是保存执行信息的场所
    std::map<std::string, func_*> funcs;
    // 版本号
    float ver_;
    // 字节码
    struct_codes byte_codes;
    // 类信息
    std::vector<class_header> class_msg;
    // 行号表，字节码对应到相应行号的表(可以选择不生成)
    std::vector<size_t> line_number_table;
    // 全局符号表大小
    size_t global_symbol_table_size;
    /**
     * @brief 用于释放字符串内存的函数
     */
    void ReleaseStringData();
    ~TVM_static_data();
    TVM_static_data();
};

#define TVM_STACK_SIZE 1024

/**
 * @brief TVM运行过程中动态数据的存放地
 * 派生类：例如：TVM， frame_
 */
class TVM_dyna_data {
public:
    // 操作数栈
    def::OBJ stack_data[TVM_STACK_SIZE];
    // 栈顶指针
    def::OBJ* stack_top_ptr;
    // 帧栈
    std::stack<frame_*> frames;
    // 全局符号表
    symbol_form global_symbol_table;
    TVM_dyna_data(size_t global_symbol_table_size);
    TVM_dyna_data();
    ~TVM_dyna_data();
    void reset_global_symbol_table(size_t size);
};
}
