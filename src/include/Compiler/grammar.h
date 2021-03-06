#pragma once

#include <Compiler/Compiler.h>
#include <Compiler/token.h>

namespace trc::compiler {
typedef std::vector<token*> code_type;

/**
 * @brief 生成语法树的解析器
 */
class TRC_Compiler_api grammar_lex {
public:
    /**
     * @brief 解析一段完整的代码成为语法树
     * @return treenode* 返回根节点指针
     */
    treenode* get_node();

    /**
     * @param codes_str 字符串代码
     * @param compiler_data 编译期间的数据和公共对象
     */
    grammar_lex(
        const std::string& codes_str, compiler_public_data& compiler_data);

    ~grammar_lex();

private:
    /**
     * @brief 检查下一个期待的token标签
     * @param tick 标签
     * @return token* 返回该符合要求的token
     */
    token* check_excepted(token_ticks tick);

    /**
     * @brief 提前获取下一个token标签
     */
    token_ticks get_next_token_tick();

    /**
     * @brief 判断数据，然后将它转为后缀表达式
     * @param raw_lex token
     * @param st 储存后缀表达式的容器
     * @param oper_tmp 储存符号的临时数据容器
     * 设置的原因是会有两个数据来源：已经读入的数据和即时读入的数据
     */
    void ConvertDataToExpressions(token* raw_lex,
        decltype(is_not_end_node::son)& st, std::stack<token_ticks>& oper_tmp,
        int& correct_braces);

    /**
     * @brief 将中缀表达式转换成后缀表达式
     * @param first_data_node 生成第一个数据的节点
     * @param first_cal_symbol 第一个运算符
     * @details 常量折叠在此进行
     */
    treenode* change_to_last_expr(treenode* first_data_node);

    /**
     * @brief 生成赋值语句节点
     * @param oper 等号的标记
     */
    treenode* assign(trc::compiler::token_ticks oper, treenode* left_value);

    /**
     * @brief 生成函数调用节点
     * @param code 整条语句
     */
    treenode* callfunction(token* funcname);

    /**
     * @brief 生成语句执行节点
     * @param code 整条语句
     */
    treenode* sentence_tree(token_ticks sentence_name);

    /**
     * @brief 生成条件循环字节码
     * @details 仅对当前行进行解析
     * @details 循环通过跳转实现
     */
    treenode* while_loop_tree();

    /**
     * @brief 生成条件判断节点
     * @details 仅对当前行进行解析
     * @details 判断通过跳转实现
     */
    treenode* if_tree();

    /**
     * @brief 生成定义函数节点
     */
    treenode* func_define();

    /**
     * @brief 优化表达式(常量折叠)和修正表达式节点
     * @param expr 表达式
     */
    void optimize_expr(is_not_end_node* expr);

    /**
     * @brief 检查后缀表达式是否正确
     */
    void check_expr(is_not_end_node* root);

    // 编译期间要用到的数据
    trc::compiler::compiler_public_data& compiler_data;

    // 数据环境
    trc::compiler::grammar_data_control* env;

    // token解析器
    trc::compiler::token_lex token_;

    // 特殊的临时终结符，临时将某token_ticks提升到和\n一样的权限,记得撤销！
    token_ticks special_tick_for_end = token_ticks::UNKNOWN;
};
}
