﻿module;
#include <algorithm>
#include <array>
#include <filesystem>
#include <language/error.hpp>
#include <obj_malloc.hpp>
module TVM;
import TRE;
import lib;
import TVM.memory;
import Error;
import TVMdef;
import lib;

namespace fs = std::filesystem;

namespace trc::TVM_space {
void TVM::LOAD_INT(bytecode_index_t index) {
    int value = static_data.const_i[index];
    if (INT_CACHE_BEGIN <= value && value <= INT_CACHE_END) {
        // 处在缓存范围中

        // 减去负数偏移量
        push(&TVM_share::int_cache[value - INT_CACHE_BEGIN]);
        return;
    }
    push(MALLOCINT(value));
}

void TVM::LOAD_MAP(bytecode_index_t argc) {
}

void TVM::LOAD_FLOAT(bytecode_index_t index) {
    push(MALLOCFLOAT(static_data.const_f[index]));
}

void TVM::LOAD_STRING(bytecode_index_t index) {
    push(MALLOCSTRING(static_data.const_s[index]));
}

void TVM::LOAD_LONG(bytecode_index_t index) {
    push(MALLOCLONG(static_data.const_long[index]));
}

void TVM::LOAD_ARRAY(bytecode_index_t index) {
}

void TVM::GOTO(bytecode_index_t index) {
    // 减一是因为结束一次循环run_index会加一，为了使索引正确需要提前减一
    run_index = index - 1;
}

/**
 * @brief 检查路径是否正确
 * @details
 * 如果路径不正确将路径转换成正确的，否则报错
 */
static void fix_path(const std::string& path) {
    if (fs::is_regular_file(path)) {
        return;
    }
    if (find(cpp_libs::names.begin(), cpp_libs::names.end(), path)
        != cpp_libs::names.end()) {
        return;
    }
    if (fs::is_regular_file(
            fs::path("TVM/packages/self_support").append(path).string())) {
        return;
    }
    if (fs::is_regular_file(
            fs::path("TVM/packages/other_support").append(path).string())) {
        return;
    }

    error::send_error(error::ModuleNotFoundError,
        language::error::modulenotfounderror, path.c_str());
}

void TVM::IMPORT() {
    // TVM *module;
    // std::string codes, file_path, name_s;
    // firsti = (def::INTOBJ) pop();
    // int n = firsti->value;
    // for (int i = 0; i < n; ++i) {
    //     firsts = (def::STRINGOBJ) pop();
    //     name_s = firsts->c_str();
    //     module = create_TVM(name_s);
    //     file_path =
    //     utils::import_to_path(name_s) +
    //     ".tree"; fix_path(file_path);

    //    utils::readcode(codes, file_path);

    //    compiler::Compiler(module, codes);
    //    modules[name_s] = module;
    //    module->run();
    //}
}
}
