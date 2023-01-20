#include <Compiler/compile_env.hpp>
#include <base/Error.hpp>
#include <language/error.hpp>

namespace trc::compiler {
basic_compile_env::basic_compile_env(
    compiler_public_data& compiler_data, TVM_space::struct_codes& bytecodes)
    : bytecode(bytecodes)
    , compiler_data(compiler_data) {
}

size_t module_compile_env::get_func_size() {
    return functions.size();
}

size_t basic_compile_env::get_name_size() {
    return var_names_list.size();
}

size_t module_compile_env::get_index_of_function(size_t name) {
    // 如果在函数列表中
    for (size_t i = 0, n = functions.size(); i < n; ++i) {
        if (functions[i] == name) {
            return i;
        }
    }
    compiler_data.error.send_error_module(error::NameError,
        language::error::nameerror, compiler_data.const_name.ref[name].c_str());
    return 0;
}

void module_compile_env::add_function(size_t name) {
    for (auto function : functions) {
        if (function == name) {
            compiler_data.error.send_error_module(error::RedefinedError,
                language::error::funcredefinederror,
                compiler_data.const_name.ref[name].c_str());
        }
    }
    functions.push_back(name);
}

size_t basic_compile_env::get_index_of_var(size_t name, bool report_error) {
    size_t n = var_names_list.size();
    for (size_t i = 0; i < n; ++i) {
        if (var_names_list[i] == name) {
            return i;
        }
    }
    if (report_error) {
        // 并不在当前符号表,报错
        compiler_data.error.send_error_module(error::NameError,
            language::error::nameerror,
            compiler_data.const_name.ref[name].c_str());
    }
    return unsave;
}

size_t basic_compile_env::add_var(size_t name) {
    for (auto i : var_names_list) {
        if (i == name) {
            // 报错
            compiler_data.error.send_error_module(error::RedefinedError,
                language::error::varredefinederror,
                compiler_data.const_name.ref[name].c_str());
        }
    }
    // 直接转移字符串所有权
    var_names_list.push_back(name);
    return var_names_list.size() - 1;
}
}
