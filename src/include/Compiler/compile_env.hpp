/**
 * 该头文件是私有头文件，不是公有头文件
 */

#pragma once

#include <Compiler/compiler_def.hpp>
#include <TVM/TVMdef.hpp>
#include <vector>

namespace trc::compiler {
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
}
