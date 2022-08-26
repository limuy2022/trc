#pragma once
#include <Compiler/pri_compiler.hpp>
#include <base/trcdef.h>

namespace trc::compiler {
/**
 * @brief
 * 关于对象的代码转换为节点以及对象信息保存和识别
 * @details
 * 其实就相当于编译时的解释器，负责解析代码并把变量的作用域和类型做出标识，用于各种判断
 */
class grammar_data_control {
public:
    /**
     * @brief 编译从map或者从array中取值的代码
     * @param head 头节点
     * @param code 读取对象代码
     */
    void compile_get_value(treenode* head, const vecs& code);

    /**
     * @brief 编译从map或者从array中创建对象的代码
     * @param head 根节点指针
     * @param code 创建对象代码
     */
    void compile_create_obj(treenode* head, const vecs& code);

    ~grammar_data_control();

private:
    // 保存数组的vector
    vecs array_list;

    // 保存map的vector，用于转成符号表索引
    vecs map_list;

    // 由于map和array获取值的方式相同但是生成的字节码不同，需要在定义时予以区分

    /**
     * @brief 生成从数组中获取值的节点
     */
    void array_get_value(treenode* head);

    /**
     * @brief 生成创建数组的节点
     */
    void create_array(treenode* head, const vecs& code);

    /**
     * @brief 生成从map中获取值的节点
     */
    void map_get_value(treenode* head);

    /**
     * @brief 生成创建map的节点
     */
    void create_map(treenode* head, const vecs& code);
};
}