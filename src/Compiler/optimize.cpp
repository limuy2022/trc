#include <Compiler/grammar.h>
#include <Compiler/pri_compiler.hpp>
#include <base/Error.h>
#include <cmath>
#include <cstring>
#include <language/error.h>
#include <stack>

/**
 * @brief 删除一个节点，同时释放它的内存
 * @param l node,链表
 * @param n 需要删除的子节点的迭代器
 */
#define DELETE_NODE(l, n)                                                      \
    do {                                                                       \
        delete (*(n));                                                         \
        (l)->son.erase((n)++);                                                 \
    } while (0)

/**
 * @brief 数字与字符串相乘
 * @param astr 指向字符串的迭代器（指针）
 * @param bint 指向数字的迭代器（指针）
 */
#define int_mul_str(astr, bint)                                                \
    do {                                                                       \
        auto anode = (node_base_string_without_sons*)*astr;                    \
        auto bnode = (node_base_int_without_sons*)*bint;                       \
        size_t len = strlen(anode->value);                                     \
        anode->value = (char*)realloc(anode->value, len * bnode->value + 1);   \
        if (anode->value == nullptr) {                                         \
            compiler_data.error.send_error_module(                             \
                error::MemoryError, language::error::memoryerror);             \
        }                                                                      \
        for (int i = 1; i <= bnode->value; ++i) {                              \
            strncpy(anode->value + len * i, anode->value, len);                \
        }                                                                      \
        anode->value[len * bnode->value] = '\0';                               \
    } while (0)

namespace trc::compiler {
// todo:加上长整型和长浮点型的常量折叠,转换等功能
// todo:加上条件表达式的常量折叠功能
void grammar_lex::optimize_expr(is_not_end_node* expr) {
    // 方法很简单，首先模拟执行过程，如果都是同种数据就进行运算压栈，不同种数据或者有变量就原封不动放入栈，一边在栈中模拟运算一边修改节点值
    std::stack<decltype(expr->son)::iterator> cal_struct;
    for (auto i = expr->son.begin(), n = expr->son.end(); i != n;) {
        // 运算符
        if ((*i)->type == grammar_type::OPCODE) {
            // 获取变量值
            auto b = pop_oper_stack(cal_struct);
            auto a = pop_oper_stack(cal_struct);
            auto t1 = (*a)->type, t2 = (*b)->type;
            if (t1 == grammar_type::VAR_NAME || t2 == grammar_type::VAR_NAME) {
                // 有一个是变量就无法优化，todo:通过上文进行值推测进行变量优化
                // todo:对于也要变量进行检查，检查是否支持该运算符操作等
                continue;
            }
            token_ticks operator_ = ((node_base_tick_without_sons*)(*i))->tick;
            // 多种情况判断优化
            // 当类型相等可以直接优化
            // 优化执行过程，释放b点，以a点作为新的节点，避免多余的分配和释放
            if (t1 == t2) {
                switch (t1) {
                case grammar_type::NUMBER: {
                    auto anode = (node_base_int_without_sons*)*a,
                         bnode = (node_base_int_without_sons*)*b;
                    switch (operator_) {
                    case token_ticks::ADD: {
                        anode->value += bnode->value;
                        break;
                    }
                    case token_ticks::SUB: {
                        anode->value -= bnode->value;
                        break;
                    }
                    case token_ticks::MUL: {
                        anode->value *= bnode->value;
                        break;
                    }
                    case token_ticks::DIV: {
                        // 先判空
                        if (!bnode->value) {
                            compiler_data.error.send_error_module(
                                error::ZeroDivError,
                                language::error::zerodiverror,
                                std::to_string(anode->value).c_str());
                        }
                        int avalue_save = anode->value;
                        delete anode;
                        auto anode_float
                            = new node_base_float_without_sons(avalue_save);
                        *a = anode_float;
                        anode_float->value /= bnode->value;
                        break;
                    }
                    default: {
                        NOREACH("Optimizer met an unexpected cal token %d",
                            (int)(operator_));
                    }
                    }
                    break;
                }
                case grammar_type::STRING: {
                    auto anode = (node_base_string_without_sons*)*a,
                         bnode = (node_base_string_without_sons*)*b;
                    switch (operator_) {
                    case token_ticks::ADD: {
                        size_t astrlen = strlen(anode->value);
                        anode->value = (char*)realloc(anode->value,
                            sizeof(char)
                                * (astrlen + strlen(bnode->value) + 1));
                        if (anode->value == nullptr) {
                            compiler_data.error.send_error_module(
                                error::MemoryError,
                                language::error::memoryerror);
                        }
                        strcpy(anode->value + astrlen, bnode->value);
                        break;
                    }
                    default: {
                        compiler_data.error.send_error_module(
                            OPERERROR_MSG(t1, t2, *i));
                    }
                    }
                    break;
                }
                case grammar_type::FLOAT: {
                    auto anode = (node_base_float_without_sons*)*a,
                         bnode = (node_base_float_without_sons*)*b;
                    switch (operator_) {
                    case token_ticks::ADD: {
                        anode->value += bnode->value;
                        break;
                    }
                    case token_ticks::SUB: {
                        anode->value -= bnode->value;
                        break;
                    }
                    case token_ticks::MUL: {
                        anode->value *= bnode->value;
                        break;
                    }
                    case token_ticks::DIV: {
                        // 判空
                        if (bnode->value == 0) {
                            compiler_data.error.send_error_module(
                                error::ZeroDivError,
                                language::error::zerodiverror,
                                std::to_string(anode->value).c_str());
                        }
                        anode->value /= bnode->value;
                        break;
                    }
                    default: {
                        NOREACH("Optimizer met an unexpected cal token %d",
                            (int)(operator_));
                    }
                    }
                    break;
                }
                default: {
                    NOREACH(
                        "Undefined type appeared in the const value optimizer");
                }
                }
                cal_struct.push(a);
                // 删除b节点
                DELETE_NODE(expr, b);
            } else {
                // 否则考虑提升类型
                // todo:列举出所有的提升方案,优化该方案的写法使其更加简洁可读
                if (t1 == grammar_type::FLOAT && t2 == grammar_type::NUMBER) {
                    auto anode = (node_base_float_without_sons*)*a;
                    auto bnode = (node_base_int_without_sons*)*b;
                    switch (operator_) {
                    case token_ticks::ADD: {
                        anode->value += bnode->value;
                        break;
                    }
                    case token_ticks::SUB: {
                        anode->value -= bnode->value;
                        break;
                    }
                    case token_ticks::MUL: {
                        anode->value *= bnode->value;
                        break;
                    }
                    case token_ticks::DIV: {
                        // anode是浮点数，直接相除
                        // 判空
                        if (bnode->value == 0) {
                            compiler_data.error.send_error_module(
                                error::ZeroDivError,
                                language::error::zerodiverror,
                                std::to_string(anode->value).c_str());
                        }
                        anode->value /= bnode->value;
                        break;
                    }
                    default: {
                        NOREACH("Optimizer met an unexpected cal token %d",
                            (int)(operator_));
                    }
                    }
                    DELETE_NODE(expr, b);
                } else if (t1 == grammar_type::NUMBER
                    && t2 == grammar_type::FLOAT) {
                    auto anode = (node_base_int_without_sons*)*a;
                    auto bnode = (node_base_float_without_sons*)*b;
                    // 保留b节点删除a节点
                    switch (operator_) {
                    case token_ticks::ADD: {
                        bnode->value += anode->value;
                        break;
                    }
                    case token_ticks::SUB: {
                        bnode->value -= anode->value;
                        bnode->value = -bnode->value;
                        break;
                    }
                    case token_ticks::MUL: {
                        bnode->value *= anode->value;
                        break;
                    }
                    case token_ticks::DIV: {
                        if (bnode->value == 0) {
                            compiler_data.error.send_error_module(
                                error::ZeroDivError,
                                language::error::zerodiverror,
                                std::to_string(anode->value).c_str());
                        }
                        bnode->value = ((double(anode->value)) / bnode->value);
                        break;
                    }
                    default: {
                        NOREACH("Optimizer met an unexpected cal token %d",
                            (int)(operator_));
                    }
                    }
                    DELETE_NODE(expr, a);
                } else if (t1 == grammar_type::STRING
                    && t2 == grammar_type::NUMBER
                    && operator_ == token_ticks::MUL) {
                    int_mul_str(a, b);
                    DELETE_NODE(expr, b);
                } else if (t1 == grammar_type::NUMBER
                    && t2 == grammar_type::STRING
                    && operator_ == token_ticks::MUL) {
                    int_mul_str(b, a);
                    DELETE_NODE(expr, a);
                } else {
                    // 类型不符合，报错
                    compiler_data.error.send_error_module(
                        OPERERROR_MSG(t1, t2, *i));
                }
            }
            // 删除当前的运算符节点
            DELETE_NODE(expr, i);
        } else {
            // 数据，直接压入栈
            cal_struct.push(i);
            ++i;
        }
    }
}
}
