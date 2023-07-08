/**
 * 该头文件是私有头文件，不是公有头文件
 */
module;
#include <language/error.hpp>
#include <limits>
#include <vector>
export module compile_env;
import compiler_def;
import TVMdef;
import Error;

export namespace trc::compiler {
// size_t最大值，代表不存在
const auto unsave = std::numeric_limits<size_t>::max();
class basic_compile_env {
public:
    explicit basic_compile_env(compiler_public_data& compiler_data,
        TVM_space::struct_codes& bytecodes);

    /**
     * @brief 获取全局的符号表的大小
     */
    size_t get_name_size();

    /**
     * @brief 将变量添加进符号表，存在该符号则报出重定义错误
     * @param name
     */
    size_t add_var(size_t name);

    /**
     * @brief 获取某个变量在符号表中的位置，有该符号则报错
     * @param name 变量名
     * @param report_error 查找不到是否直接报错
     */
    size_t get_index_of_var(size_t name, bool report_error);

    // 字节码保存处
    TVM_space::struct_codes& bytecode;

protected:
    typedef std::vector<size_t> name_list_t;
    // 记录全局的变量名
    name_list_t var_names_list;
    // 编译期间要用到的公共数据
    compiler_public_data& compiler_data;
};

/**
 * @brief 编译时的环境，主要记录各种变量的信息和作用域，可用于优化和计算行号表
 * @details 该类每个模块都配备一个
 */
class module_compile_env : public basic_compile_env {
public:
    using basic_compile_env::basic_compile_env;

    /**
     * @brief 获取某个函数在符号表中的位置
     * @param name 变量名
     */
    size_t get_index_of_function(size_t name);

    /**
     * @brief 将函数添加进符号表，负责报出重定义错误
     * @param name 函数名
     */
    void add_function(size_t name);

    size_t get_func_size();

private:
    // 函数名
    name_list_t functions;
};

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
