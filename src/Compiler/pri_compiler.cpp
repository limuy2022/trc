/**
 * 编译时期共享的数据
 */

#include "Compiler/pri_compiler.hpp"
#include "base/Error.h"
#include "language/error.h"
#include <array>
#include <cstring>
#include <map>
#include <string>

namespace trc::compiler {
treenode::~treenode() = default;

void is_not_end_node::connect(treenode* son_node) {
    son.push_back(son_node);
}

bool is_not_end_node::has_son() {
    return true;
}

bool is_end_node::has_son() {
    return false;
}

tick_node::tick_node(token_ticks tick)
    : tick(tick) {
}

data_node::data_node(const std::string& data)
    : data(
        (char*)malloc(sizeof(char) * (data.length() + 1))) {
    strcpy(this->data, data.c_str());
}

void data_node::set(const std::string& new_data) {
    set_alloc(new_data.length());
    strcpy(this->data, new_data.c_str());
}

void data_node::set(const char* new_data) {
    set_alloc(strlen(new_data));
    strcpy(this->data, new_data);
}

data_node::~data_node() {
    // 直接释放是因为可以释放空指针
    free(data);
}

void data_node::set_alloc(size_t sizes) {
    // 直接使用realloc是因为当传入的指针为空时相当于malloc
    data = (char*)realloc(data, sizeof(char) * (sizes + 1));
    if (data == nullptr) {
        error::send_error(error::MemoryError,
            language::error::memoryerror);
    }
}

node_base_data::node_base_data(
    grammar_type type_argv, const std::string& data)
    : data_node(data) {
    this->type = type_argv;
}

node_base_data::node_base_data(grammar_type type) {
    this->type = type;
}

node_base_tick::node_base_tick(
    grammar_type type, token_ticks tick)
    : tick_node(tick) {
    this->type = type;
}

node_base_data_without_sons::node_base_data_without_sons(
    grammar_type type) {
    this->type = type;
}

node_base_data_without_sons::node_base_data_without_sons(
    grammar_type type, const std::string& data)
    : data_node(data) {
    this->type = type;
}

node_base_int_without_sons::node_base_int_without_sons(
    grammar_type type, int value)
    : value(value) {
    this->type = type;
}

COMPILE_TYPE_TICK what_type(const std::string& value) {
    char front = value.front(), back = value.back();
    size_t lenght = value.length();
    if (('\'' == front && '\'' == back)
        || (front == '"' && back == '"'))
        return string_TICK;

    if (isdigit(front)) {
        if (value.find('.') != std::string::npos) {
            // 小数
            if (lenght >= 12)
                return compiler::FLOAT_L_TICK;
            return compiler::float_TICK;
        }
        // 检查数据,前导零
        if (lenght >= 2 && front == '0')
            error::send_error(error::SyntaxError,
                language::error::syntaxerror_int,
                value.c_str());
        if (lenght >= 12)
            return LONG_TICK;

        return int_TICK;
    }
    if (utils::check_in(value, const_values))
        return CONST_TICK;
    return VAR_TICK;
}

// 可以优化的运算符
std::array<std::string, 7> opti_opers
    = { "+", "-", "**", "*", "//", "%", "/" };
// 可以优化的条件运算符
std::array<std::string, 8> opti_condits
    = { "<", ">", "<=", ">=", "!=", "==", "and", "or" };
// 转义字符对应表
std::map<char, std::string> change_varchar
    = { { 'n', "\n" }, { 't', "\t" }, { '\'', "'" },
          { '"', R"(")" }, { '\\', R"(\)" } };
// 常量值
std::array<std::string, 3> const_values
    = { "true", "false", "null" };
// 常量值到数字的转换
std::map<std::string, int> change_const = {
    { "true", 1 },
    { "false", 0 },
    { "null", 0 },
};

// 优先级
const cal_priority_class cal_priority;

// 优先级列表，越小优先级越低
const int cal_priority_class::map_[] = {
    3, // +
    3, // -
    4, // *
    4, // /
    4, // //
    4, // %
    5, // **
    0, // and
    0, // or
    2, // not
    1, // ==
    1, // !=
    1, // <
    1, // >
    1, // <=
    1, // >=
};

int cal_priority_class::operator[](token_ticks tick) const {
    return map_[(int)tick - (int)token_ticks::ADD];
}
}
