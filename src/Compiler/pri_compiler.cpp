/**
 * 编译各个阶段的公用函数和变量
 */

#include <Compiler/pri_compiler.hpp>
#include <array>
#include <base/Error.h>
#include <cassert>
#include <cstring>
#include <language/error.h>
#include <map>
#include <string>

namespace trc::compiler {
treenode::~treenode() = default;

void is_not_end_node::connect(treenode* son_node) {
    son.push_back(son_node);
}

is_not_end_node::is_not_end_node() {
    this->has_son = true;
}

is_not_end_node::is_not_end_node(grammar_type type) {
    new (this) is_not_end_node;
    this->type = type;
}

is_end_node::is_end_node() {
    this->has_son = false;
}

tick_node::tick_node(token_ticks tick)
    : tick(tick) {
}

data_node::data_node(const char* data)
    : data((char*)malloc(sizeof(char) * (strlen(data) + 1))) {
    strcpy(this->data, data);
}

void data_node::set(const char* new_data) {
    set_alloc(strlen(new_data));
    strcpy(this->data, new_data);
}

void data_node::swap_token_data(token* token_value) {
    this->data = token_value->data;
    token_value->data = nullptr;
}

data_node::~data_node() {
    // 直接释放是因为可以释放空指针
    free(data);
}

void data_node::set_alloc(size_t sizes) {
    // 直接使用realloc是因为当传入的指针为空时相当于malloc
    data = (char*)realloc(data, sizeof(char) * (sizes + 1));
    if (data == nullptr) {
        error::send_error(error::MemoryError, language::error::memoryerror);
    }
    data[sizes] = '\0';
}

char* data_node::swap_string_data() {
    // 防止被多次调用
    assert(data != nullptr);
    char* str_data_ptr = data;
    // 置空是使释放时不会释放掉内存
    data = nullptr;
    return str_data_ptr;
}

node_base_data::node_base_data(grammar_type type_argv, const char* data)
    : data_node(data) {
    this->type = type_argv;
}

node_base_tick::node_base_tick(grammar_type type, token_ticks tick)
    : tick_node(tick) {
    this->type = type;
}

node_base_data_without_sons::node_base_data_without_sons(
    grammar_type type, token* data) {
    this->type = type;
    this->swap_token_data(data);
}

node_base_data_without_sons::node_base_data_without_sons() = default;

node_base_tick_without_sons::node_base_tick_without_sons(
    grammar_type type, token_ticks tick)
    : tick_node(tick) {
    this->type = type;
}

node_base_int_without_sons::node_base_int_without_sons(
    int value, grammar_type type)
    : value(value) {
    this->type = type;
}

node_base_int::node_base_int(int value, grammar_type type)
    : value(value) {
    this->type = type;
}

node_base_float_without_sons::node_base_float_without_sons(double value)
    : value(value) {
    this->type = grammar_type::FLOAT;
}

// 常量值到数字的转换
std::map<std::string, int> change_const = {
    { "true", 1 },
    { "false", 0 },
    { "null", 0 },
};

// 优先级列表，越小优先级越低
int map_[] = {
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

const char* name_map_token[] = {
    "+",
    "-",
    "*",
    "/",
    "//",
    "%",
    "**",
    "and",
    "or",
    "not",
    "==",
    "!=",
    "<",
    ">",
    "<=",
    ">=",
};

const char* name_map_grammar[] = {
    "int",
    "float",
    "long int",
    "long float",
    "string",
};

const next_order_map<int, token_ticks> cal_priority(map_, token_ticks::ADD);

const next_order_map<const char*, grammar_type> str_grammar_type_cal_map(
    name_map_grammar, grammar_type::NUMBER);

const next_order_map<const char*, token_ticks> str_token_ticks_cal_map(
    name_map_token, token_ticks::ADD);
}
