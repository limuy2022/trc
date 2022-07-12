#include <TVM/TVM_data.h>
#include <cstdlib>

namespace trc::TVM_space {
symbol_form::~symbol_form() {
    free(vars);
}

symbol_form::symbol_form(size_t size)
    : vars((def::OBJ*)malloc(sizeof(def::OBJ) * size)) {
}

void symbol_form::reset(size_t size) {
    vars = (def::OBJ*)(realloc(vars, size * sizeof(def::OBJ)));
}

void TVM_dyna_data::reset_global_symbol_table(size_t size) {
    global_symbol_table.reset(size);
}

#define TVM_STACK_SIZE 1024

TVM_dyna_data::TVM_dyna_data(size_t global_symbol_table_size)
    : global_symbol_table(global_symbol_table_size) {
    new (this) TVM_dyna_data;
}

TVM_dyna_data::TVM_dyna_data()
    : stack_data((def::OBJ*)malloc(sizeof(def::OBJ) * TVM_STACK_SIZE))
    , stack_top_ptr(stack_data - 1) {
}

TVM_dyna_data::~TVM_dyna_data() {
    free(stack_data);
}

void TVM_static_data::ReleaseStringData() {
    for (auto i = const_s.begin() + 1, end = const_s.end(); i != end; ++i) {
        delete[] * i;
    }
    const_s.clear();
    const_s.push_back(nullptr);
    for (auto i = const_long.begin() + 1, end = const_long.end(); i != end;
         ++i) {
        delete[] * i;
    }
    const_long.clear();
    const_long.push_back(nullptr);
}

TVM_static_data::~TVM_static_data() {
    ReleaseStringData();
}

// 初始化为1个是因为索引为uint16_t，所以0就表示无参数，0号位必须放一个数占位
TVM_static_data::TVM_static_data()
    : const_i(1)
    , const_s(1)
    , const_long(1)
    , const_f(1) {
}
}
