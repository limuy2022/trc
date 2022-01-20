#include "Compiler/grammar.h"
#include <cstdarg>
#include <gtest/gtest.h>

using namespace trc::compiler;

/**
 * @brief 检查语法树是否正确
 * @param code trc代码
 * @param treenums 有多少个节点
 * @param 可变参数
 * 传入treenode*类型，为语法树的头部，有treenums个
 */
static void check_grammar(
    const char* code, size_t treenums, ...) {
}

TEST(grammar, var_lex) {
}
