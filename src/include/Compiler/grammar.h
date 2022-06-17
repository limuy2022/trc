#pragma once

#include <Compiler/Compiler.h>

namespace trc::compiler {
/**
 * 语言的解析细节
 */
class TRC_Compiler_api grammar_lex {
    typedef std::vector<token*> code_type;

public:
    /**
     * @brief 解析一段完整的代码成为语法树
     * @param other_end_sym
     * 除END_OF_TOKEN和END_OF_LINE以外的终结符号
     * @return treenode* 返回根节点指针
     */
    treenode* get_node();

    /**
     * @param line 储存代码行的地方
     * @param error_ 报错模块
     */
    grammar_lex(
        const std::string& codes_str, trc::compiler::compiler_error* error_);

    ~grammar_lex();

private:
    token* check_excepted(token_ticks tick);

    /**
     * @brief 判断数据，然后将它转为后缀表达式
     * @param raw_lex token
     * @param st 储存后缀表达式的容器
     * @param oper_tmp 储存符号的临时数据容器
     * 设置的原因是会有两个数据来源：已经读入的数据和即时读入的数据
     */
    void ConvertDataToExpressions(token* raw_lex, std::vector<treenode*>& st,
        std::stack<token_ticks>& oper_tmp);

    /**
     * @brief 将中缀表达式转换成后缀表达式
     * @details 常量折叠在此进行
     */
    void change_to_last_expr(is_not_end_node* head, code_type& code);

    /**
     * @brief 生成赋值语句节点
     * @param head 根节点指针
     * @param oper 等号的标记
     * @param lvalue 左值
     * @param rvalue 右值
     */
    void assign(is_not_end_node* head, trc::compiler::token_ticks oper,
        const code_type& code);

    /**
     * @brief 生成函数调用节点
     * @param head 根节点指针
     * @param code 整条语句
     */
    void callfunction(node_base_data* functree, const code_type& funcname);

    /**
     * @brief 生成语句执行节点
     * @param head 根节点指针
     * @param code 整条语句
     */
    void sentence_tree(node_base_tick* head, token_ticks sentence_name);

    /**
     * @brief 生成条件循环字节码
     * @param head 根节点指针
     * @param code 整条语句
     * @details 仅对当前行进行解析
     * @details 循环通过跳转实现
     */
    void while_loop_tree(is_not_end_node* head);

    /**
     * @brief 生成条件判断节点
     * @param head 根节点指针
     * @param code 整条语句
     * @details 仅对当前行进行解析
     * @details 判断通过跳转实现
     */
    void if_tree(is_not_end_node* head);

    /**
     * @brief 生成定义函数节点
     * @param head 根节点指针
     * @param code 整条语句
     */
    void func_define(is_not_end_node* head);

    trc::compiler::compiler_error* error_;

    // 数据环境
    trc::compiler::grammar_data_control* env;

    trc::compiler::token_lex token_;
};
}
