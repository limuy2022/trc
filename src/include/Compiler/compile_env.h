/**
 * 该头文件是私有头文件，不是公有头文件
 */

#pragma once

#include <Compiler/compiler_def.h>
#include <Compiler/library.h>
#include <Compiler/pri_compiler.hpp>
#include <base/Error.h>
#include <map>
#include <string>
#include <vector>

namespace trc::compiler {
/**
 * @brief 编译时的环境，主要记录各种变量的信息和作用域，可用于优化和计算行号表
 */
class TRC_Compiler_api CompileEnvironment {
public:
    explicit CompileEnvironment(compiler_public_data& compiler_data);

    ~CompileEnvironment();
    /**
     * @brief 获取全局的符号表的大小
     */
    size_t get_global_name_size();

    /**
     * @brief 获取局部的符号表的大小
     */
    size_t get_local_name_size(const std::string& name);

    /**
     * @brief 获取某个变量在全局符号表中的位置，没有该符号则添加进符号表
     * @param name 变量名
     * @param maybe_not_in
     * 允许该变量不在符号表中吗，允许则会添加进符号表，不允许则会报错
     */
    size_t get_index_of_globalvar(char* name, bool maybe_not_in);

    /**
     * @brief 获取某个变量在局部变量表中的位置，没有该符号则添加进符号表
     * @param localspace_name 局部空间名，如函数名
     * @param name 变量名
     * @param maybe_not_in
     * 允许该变量不在符号表中吗，允许则会添加进符号表，不允许则会报错
     */
    size_t get_index_of_localvar(
        const std::string& localspace_name, char* name, bool maybe_not_in);

private:
    typedef std::vector<const char*> name_list_t;
    // 记录全局的变量名
    name_list_t var_names_list_global;
    // 记录局部的变量名
    std::map<std::string, name_list_t> var_names_list_local;
    // 编译期间要用到的公共数据
    compiler_public_data& compiler_data;

    /**
     * @brief 从某列表中查找索引和添加数据
     * @warning 如果未添加进列表中，会直接释放内存
     */
    size_t get_index_from_list(
        char* name, name_list_t& list, bool maybe_not_in);
};
}
