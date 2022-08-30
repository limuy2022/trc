/**
 * 该头文件是私有头文件，不是公有头文件
 */

#pragma once

#include <Compiler/compiler_def.h>
#include <Compiler/pri_compiler.hpp>
#include <base/Error.h>
#include <language/error.h>
#include <map>
#include <string>
#include <vector>

namespace trc::compiler {
inline size_t size_tmax = -1;

/**
 * @brief 编译时的环境，主要记录各种变量的信息和作用域，可用于优化和计算行号表
 * @details 该类每个模块都配备一个
 */
class CompileEnvironment {
public:
    explicit CompileEnvironment(compiler_public_data& compiler_data);

    ~CompileEnvironment();
    /**
     * @brief 获取全局的符号表的大小
     */
    size_t get_name_size();

    /**
     * @brief 将变量添加进符号表，存在该符号则报出重定义错误
     * @param name
     */
    size_t add_var(char* name);

    /**
     * @brief 获取某个变量在符号表中的位置，有该符号则报错
     * @param name 变量名
     * @param report_error 查找不到是否直接报错
     */
    size_t get_index_of_var(char* name, bool report_error);

    /**
     * @brief 获取某个函数在符号表中的位置
     * @param name 变量名
     */
    size_t get_index_of_function(const char* name);

    /**
     * @brief 将函数添加进符号表，负责报出重定义错误
     */
    void add_function(const char* name);

private:
    typedef std::vector<const char*> name_list_t;
    // 记录全局的变量名
    name_list_t var_names_list;
    // 编译期间要用到的公共数据
    compiler_public_data& compiler_data;
    // 函数名
    name_list_t functions;
};
}
