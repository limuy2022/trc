/**
 * 该头文件是私有头文件，不是公有头文件
 */

#pragma once

#include <map>
#include <string>
#include <vector>

namespace trc::compiler {
/**
 * @brief 编译时的环境，主要记录各种变量的信息和作用域，可用于优化和计算行号表
 */
class CompileEnvironment {
public:
    CompileEnvironment();

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
     */
    size_t get_index_of_globalvar(const char* name);

    /**
     * @brief 获取某个变量在局部变量表中的位置，没有该符号则添加进符号表
     */
    size_t get_index_of_localvar(
        const std::string& localspace_name, const char* localvar_name);

private:
    typedef std::vector<const char*> name_list_t;
    // 记录全局的变量名
    name_list_t var_names_list_global;
    // 记录局部的变量名
    std::map<std::string, name_list_t> var_names_list_local;
};
}
