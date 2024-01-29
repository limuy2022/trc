/**
 * TVM各种数据集合的定义处
 */

module;
#include <string>
#include <vector>
export module TVM_data;
import TVMdef;
import func;
import object;
import trcdef;

export namespace trc::TVM_space {
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

class frame_ {
public:
    void set_func(const func_& func);
    void free_func();
    // 保存函数的名字
    const char* funcname;
    // 符号表
    symbol_form localvars;
};

void frame_::set_func(const trc::TVM_space::func_& func) {
}

void frame_::free_func() {
}

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

void TVM_dyna_data::reset_global_symbol_table(size_t size) {
    global_symbol_table.reset(size);
}

TVM_dyna_data::TVM_dyna_data()
    : stack_top_ptr(stack_data - 1)
    , frames_top_str(frames - 1) {
}

void TVM_static_data::compress_memory() {
    const_i.shrink_to_fit();
    const_s.shrink_to_fit();
    const_long.shrink_to_fit();
    const_f.shrink_to_fit();
    funcs.shrink_to_fit();
}
}
