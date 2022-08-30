#include <Compiler/compile_env.h>
#include <algorithm>
#include <base/Error.h>
#include <cstring>
#include <language/error.h>

namespace trc::compiler {
CompileEnvironment::CompileEnvironment(compiler_public_data& compiler_data)
    : compiler_data(compiler_data) {
}

CompileEnvironment::~CompileEnvironment() {
    // 释放掉变量名内存
    for (auto i : var_names_list) {
        delete[] i;
    }
    for (auto i : functions) {
        delete[] i;
    }
}

size_t CompileEnvironment::get_name_size() {
    return var_names_list.size();
}

size_t CompileEnvironment::get_index_of_function(const char* name) {
    // 如果在函数列表中
    for (size_t i = 0, n = functions.size(); i < n; ++i) {
        if (!strcmp(functions[i], name)) {
            return i;
        }
    }
    compiler_data.error.send_error_module(
        error::NameError, language::error::nameerror, name);
    return 0;
}

void CompileEnvironment::add_function(const char* name) {
    size_t n = functions.size();
    for (size_t i = 0; i < n; ++i) {
        if (!strcmp(functions[i], name)) {
            compiler_data.error.send_error_module(error::RedefinedError,
                language::error::funcredefinederror, name);
        }
    }
    functions.push_back(name);
}

size_t CompileEnvironment::get_index_of_var(char* name, bool report_error) {
    size_t n = var_names_list.size();
    for (size_t i = 0; i < n; ++i) {
        if (!strcmp(var_names_list[i], name)) {
            return i;
        }
    }
    if (report_error) {
        // 并不在当前符号表,报错
        compiler_data.error.send_error_module(
            error::NameError, language::error::nameerror, name);
    }
    // -1转换到无符号整型中是最大值，代表不存在
    return size_tmax;
}

size_t CompileEnvironment::add_var(char* name) {
    size_t n = var_names_list.size();
    for (size_t i = 0; i < n; ++i) {
        if (!strcmp(var_names_list[i], name)) {
            // 报错
            compiler_data.error.send_error_module(error::RedefinedError,
                language::error::varredefinederror, name);
        }
    }
    // 直接转移字符串所有权
    var_names_list.push_back(name);
    return n;
}
}
