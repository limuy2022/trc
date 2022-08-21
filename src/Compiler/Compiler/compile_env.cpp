#include <Compiler/compile_env.h>
#include <base/Error.h>
#include <cstring>
#include <language/error.h>

namespace trc::compiler {
CompileEnvironment::CompileEnvironment(compiler_public_data& compiler_data)
    : compiler_data(compiler_data) {
}

CompileEnvironment::~CompileEnvironment() {
    // 释放掉变量名内存
    for (auto i : var_names_list_global) {
        delete[] i;
    }
}

size_t CompileEnvironment::get_global_name_size() {
    return var_names_list_global.size();
}

size_t CompileEnvironment::get_local_name_size(const std::string& name) {
    return var_names_list_local[name].size();
}

size_t CompileEnvironment::get_index_from_list(
    char* name, name_list_t& list, bool maybe_not_in) {
    size_t n = list.size();
    for (size_t i = 0; i < n; ++i) {
        if (!strcmp(list[i], name)) {
            free(name);
            return i;
        }
    }
    // 并不在当前符号表
    if (maybe_not_in) {
        // 允许不在符号表
        // 添加进符号表
        // 直接转移字符串所有权
        list.push_back(name);
        return n;
    } else {
        // 报错
        compiler_data.error.send_error_module(
            error::NameError, language::error::nameerror, name);
        return 0;
    }
}

size_t CompileEnvironment::get_index_of_globalvar(
    char* node, bool maybe_not_in) {
    return get_index_from_list(node, var_names_list_global, maybe_not_in);
}

size_t CompileEnvironment::get_index_of_localvar(
    const std::string& localspace_name, char* node, bool maybe_not_in) {
    return get_index_from_list(
        node, var_names_list_local[localspace_name], maybe_not_in);
}
}
