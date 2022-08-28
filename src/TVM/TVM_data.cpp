﻿#include <TVM/TVM_data.h>
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

TVM_dyna_data::TVM_dyna_data()
    : stack_top_ptr(stack_data - 1)
    , frames_top_str(frames - 1) {
}

void TVM_static_data::ReleaseStringData() {
    for (auto& i : const_s) {
        free(const_cast<char*>(i));
    }
    const_s.clear();
    for (auto& i : const_long) {
        free(const_cast<char*>(i));
    }
    const_long.clear();
}

TVM_static_data::~TVM_static_data() {
    ReleaseStringData();
}
}
