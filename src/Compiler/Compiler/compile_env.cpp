#include "compile_env.h"
#include <cstring>

namespace trc::compiler {
size_t CompileEnvironment::get_global_name_size() {
    return var_names_list_global.size();
}

size_t CompileEnvironment::get_local_name_size(const std::string& name) {
    return var_names_list_local[name].size();
}

size_t CompileEnvironment::get_index_of_globalvar(const char* name) {
    size_t n = var_names_list_global.size();
    for (size_t i = 0; i < n; ++i) {
        if (!strcmp(var_names_list_global[i], name)) {
            return i;
        }
    }
    // 并不在当前符号表
    // 添加进符号表
    var_names_list_global.push_back(name);
    return n;
}

size_t CompileEnvironment::get_index_of_localvar(
    const std::string& localspace_name, const char* localvar_name) {
    auto& l = var_names_list_local[localspace_name];
    size_t n = l.size();
    for (size_t i = 0; i < n; i++) {
        if (!strcmp(l[i], localvar_name)) {
            return i;
        }
    }
    l.push_back(localvar_name);
    return n;
}
}
