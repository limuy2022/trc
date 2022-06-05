#include <Compiler/grammar.h>
#include <cstdarg>
#include <gtest/gtest.h>
#include <string>

using namespace trc::compiler;

static compiler_error error("grammar_unittest");

/**
 * @brief 检查语法树是否正确
 * @param code trc代码
 * @param treenums 有多少个节点
 * @param 可变参数
 * 传入treenode*类型，为语法树的头部，有treenums个
 */
static void check_grammar(
    const char* code, treenode* treenode) {
    grammar_lex grammar_test(code, &error);
}

// 测试变量解析是否正确
TEST(grammar, var_lex) {
    is_not_end_node* HEAD = new is_not_end_node;
    auto assign = new node_base_tick(
        grammar_type::TREE, token_ticks::ASSIGN);
    HEAD->connect(assign);
    check_grammar("a:=10", HEAD);
}
