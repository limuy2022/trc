#include <Compiler/compile_env.h>
#include <base/Error.h>
#include <cstring>
#include <language/error.h>
#include <limits>

namespace trc::compiler {
basic_compile_env::basic_compile_env(
    compiler_public_data& compiler_data, TVM_space::struct_codes& bytecodes)
    : bytecode(bytecodes)
    , compiler_data(compiler_data) {
}

basic_compile_env::~basic_compile_env() {
    // 释放掉变量名内存
    for (auto i : var_names_list) {
        delete[] i;
    }
}

module_compile_env::~module_compile_env() {
    for (auto i : functions) {
        delete[] i;
    }
}

size_t module_compile_env::get_func_size() {
    return functions.size();
}

size_t basic_compile_env::get_name_size() {
    return var_names_list.size();
}

size_t module_compile_env::get_index_of_function(
    const char* name, line_t line) {
    // 如果在函数列表中
    for (size_t i = 0, n = functions.size(); i < n; ++i) {
        if (!strcmp(functions[i], name)) {
            return i;
        }
    }
    compiler_data.error.send_error_module(
        { error::NameError, line }, language::error::nameerror, name);
    return 0;
}

void module_compile_env::add_function(const char* name, line_t line) {
    for (auto function : functions) {
        if (!strcmp(function, name)) {
            compiler_data.error.send_error_module(
                { error::RedefinedError, line },
                language::error::funcredefinederror, name);
        }
    }
    functions.push_back(name);
}

size_t basic_compile_env::get_index_of_var(char* name, bool report_error, line_t line) {
    size_t n = var_names_list.size();
    for (size_t i = 0; i < n; ++i) {
        if (!strcmp(var_names_list[i], name)) {
            return i;
        }
    }
    if (report_error) {
        // 并不在当前符号表,报错
        compiler_data.error.send_error_module(
            { error::NameError, line }, language::error::nameerror,
            name);
    }
    // size_t最大值，代表不存在
    return std::numeric_limits<size_t>::max();
}

size_t basic_compile_env::add_var(char* name, line_t line) {
    for (auto i : var_names_list) {
        if (!strcmp(i, name)) {
            // 报错
            compiler_data.error.send_error_module(
                { error::RedefinedError, line },
                language::error::varredefinederror, name);
        }
    }
    // 直接转移字符串所有权
    var_names_list.push_back(name);
    return var_names_list.size() - 1;
}
}
