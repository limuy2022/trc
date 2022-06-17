/**
 * TVM各种数据集合的定义处
 */

#pragma once

#include "TVMdef.h"
#include "library.h"
#include "object.h"
#include <map>
#include <stack>
#include <string>
#include <vector>

namespace trc::TVM_space {
class func_;

class frame_;

struct TVM_bytecode {
    bytecode_t bycode;
    bytecode_index_t index;
};

/**
 * @brief 静态数据
 * 本类用于分离TVM的职责，装载编译时的数据，如常量池等
 * 并且降低TVM和Compiler之间的耦合度
 */
struct TRC_TVM_api TVM_static_data {
    // 各种常量数据
    std::vector<int> const_i;
    std::vector<char*> const_s, const_name, const_long;
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
    void ReleaseStringData();
    ~TVM_static_data();
    TVM_static_data();
};

struct TVM_dyna_data {
    /**
     * TVM运行过程中动态数据的存放地
     * 派生类：例如：TVM， frame_
     */

    // 变量
    std::map<std::string, def::OBJ> var_names;
    // 操作数栈
    std::stack<def::OBJ> stack_data;
    // 帧栈
    std::stack<frame_*> frames;
};

class symbol_form {
    /**
     * 符号表，保存变量信息
     * 就是凭借这个结构优化变量访问速度为O(1)
     */
public:
    int* vars;

    ~symbol_form();
};
}
