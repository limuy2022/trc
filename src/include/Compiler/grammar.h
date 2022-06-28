#pragma once

#include <Compiler/Compiler.h>

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
     * @brief 解析一段完整的代码成为语法树
     * @return treenode* 返回根节点指针
     * @details 该函数接受一段tokens，从这段tokens中获取token而不是从token解析器
     * @warning 传入的token需要自己释放，该函数不会为你释放内存
     */
    treenode* get_node(const code_type& code);

    /**
     * @param line 储存代码行的地方
     * @param error_ 报错模块
     */
    grammar_lex(
        const std::string& codes_str, trc::compiler::compiler_error* error_);

    ~grammar_lex();

private:
    /**
     * @brief 封装获取token的函数接口
     */
    class get_token_interface {
    public:
        virtual token* get_token() = 0;

        virtual void unget_token(token*) = 0;
    };

    /**
     * @brief 从数据获取token
     */
    class get_token_from_token_lex : public get_token_interface {
    public:
        token* get_token();

        void unget_token(token*);

        void reload(token_lex& token_);

    private:
        token_lex* token_;
    } token_from_token_lex;

    /**
     * @brief 从token_lex获取token
     */
    class get_token_from_data : public get_token_interface {
    public:
        token* get_token();
        void unget_token(token*);

        void reload(const code_type& data);

    private:
        // 记录数据读取到哪里了
        size_t index;
        // 记录数据源
        const code_type* data;
    } token_from_data;

    // 由于在同一时间只能使用一种获取token的方式，所以用它来统一获取token的方式
    get_token_interface* token_way;

    /**
     * @brief get_node的内部接口实现
     */
    treenode* get_node_interal();

    token* check_excepted(token_ticks tick);

    /**
     * @brief 判断数据，然后将它转为后缀表达式
     * @param raw_lex token
     * @param st 储存后缀表达式的容器
     * @param oper_tmp 储存符号的临时数据容器
     * 设置的原因是会有两个数据来源：已经读入的数据和即时读入的数据
     */
    void ConvertDataToExpressions(token* raw_lex, std::vector<treenode*>& st,
        std::stack<token_ticks>& oper_tmp, int& correct_braces);

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
    treenode* callfunction(const code_type& funcname);

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
