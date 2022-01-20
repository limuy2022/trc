#include "Compiler/Compiler.h"
#include "TVM/TVM.h"
#include "TVM/lib.h"
#include "TVMbase/TRE.h"
#include "TVMbase/memory.h"
#include "base/Error.h"
#include "base/trcdef.h"
#include "base/utils/data.hpp"
#include "base/utils/filesys.h"
#include "language/error.h"

namespace trc::TVM_space {
void TVM::LOAD_INT(short index) {
    int value = static_data.const_i[index];
    if (INT_CACHE_BEGIN <= value
        && value <= INT_CACHE_END) {
        // 处在缓存范围中

        // 减去负数偏移量
        push(
            &TVM_share::int_cache[value - INT_CACHE_BEGIN]);
        return;
    }
    push(MALLOCINT(value));
}

void TVM::LOAD_MAP(short argc) {
}

void TVM::LOAD_FLOAT(short index) {
    push(MALLOCFLOAT(static_data.const_f[index]));
}

void TVM::LOAD_STRING(short index) {
    push(MALLOCSTRING(static_data.const_s[index]));
}

void TVM::LOAD_LONG(short index) {
    push(MALLOCLONG(static_data.const_long[index]));
}

void TVM::LOAD_ARRAY(short index) {
}

void TVM::NOP() {
}

void TVM::GOTO(short index) {
    run_index = static_data.const_i[index] - 1;
}

/**
 * @brief 检查路径是否正确
 * @details
 * 如果路径不正确将路径转换成正确的，否则报错
 */
static void fix_path(const std::string& path) {
    if (utils::file_exists(path)) {
        return;
    }
    if (utils::check_in(path, cpp_libs::names)) {
        return;
    }
    if (utils::file_exists(utils::path_join(2,
            "TVM/packages/self_support", path.c_str()))) {
        return;
    }
    if (utils::file_exists(utils::path_join(2,
            "TVM/packages/other_support", path.c_str()))) {
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
