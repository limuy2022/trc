#include <Compiler/grammar.h>
#include <Compiler/optimize.h>
#include <Compiler/pri_compiler.hpp>
#include <base/Error.h>
#include <base/utils/data.hpp>
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
        delete (*n);                                                           \
        (l)->son.erase((n));                                                   \
    } while (0)

namespace trc::compiler {
// todo:加上长整型和长浮点型的常量折叠,转换等功能
// todo:加上条件表达式的常量折叠功能
void grammar_lex::optimize_expr(is_not_end_node* expr) {
    // 方法很简单，首先模拟执行过程，如果都是同种数据就进行运算压栈，不同种数据或者有变量就原封不动放入栈，一边在栈中模拟运算一边修改节点值
    std::stack<decltype(expr->son)::iterator> cal_struct;
    for (auto i = expr->son.begin(), n = expr->son.end(); i != n; ++i) {
        if ((*i)->type == grammar_type::OPCODE) {
            // 运算符
            auto a = cal_struct.top();
            cal_struct.pop();
            auto b = cal_struct.top();
            cal_struct.pop();
            auto t1 = (*a)->type, t2 = (*b)->type;
            if (t1 == grammar_type::VAR_NAME || t2 == grammar_type::VAR_NAME) {
                // 有一个是变量就无法优化，todo:通过上文进行值推测进行变量优化
                // todo:对于也要变量进行检查，检查是否支持该运算符操作等
                continue;
            }
            // 多种情况判断优化
            // 当类型相等可以直接优化
            if (t1 == t2) {
                switch (t1) {
                case grammar_type::NUMBER: {
                    auto anode = ((node_base_int_without_sons*)*a),
                         bnode = ((node_base_int_without_sons*)*b);
                    // 优化执行过程，释放b点，以a点作为新的节点，避免多余的分配和释放
                    token_ticks operator_
                        = ((node_base_tick_without_sons*)(*i))->tick;
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
                        // todo:提升为浮点型
                        break;
                    }
                    default: {
                        NOREACH("Optimizer met an unexpected cal token %d",
                            (int)(operator_));
                    }
                    }
                    DELETE_NODE(expr, b);
                    break;
                }
                case grammar_type::STRING: {
                    auto atmp = (node_base_string_without_sons*)*a,
                         btmp = (node_base_string_without_sons*)*b;
                    size_t astrlen = strlen(atmp->data);
                    atmp->data = (char*)realloc(atmp->data,
                        sizeof(char) * (astrlen + strlen(btmp->data) + 1));
                    strcpy(atmp->data + astrlen, btmp->data);
                    DELETE_NODE(expr, b);
                    break;
                }
                case grammar_type::FLOAT: {
                    auto anode = ((node_base_float_without_sons*)*a),
                         bnode = ((node_base_float_without_sons*)*b);
                    anode->value += bnode->value;
                    DELETE_NODE(expr, b);
                    break;
                }
                }
            } else {
                // 否则考虑提升类型
                // todo:列举出所有的提升方案,优化该方案的写法使其更加简洁可读
                // 这两种都是整型配浮点型
                if (t1 == grammar_type::FLOAT && t2 == grammar_type::NUMBER) {

                } else if (t1 == grammar_type::NUMBER
                    && t2 == grammar_type::FLOAT) {

                } else {
                    // 类型不符合，报错
                    compiler_data.error.send_error_module(
                        OPERERROR_MSG(t1, t2, *i));
                }
            }
            DELETE_NODE(expr, i);
        } else {
            // 数据
            cal_struct.push(i);
        }
    }
}
}
