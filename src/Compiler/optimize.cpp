#include <Compiler/grammar.h>
#include <Compiler/pri_compiler.hpp>
#include <base/Error.h>
#include <cmath>
#include <cstring>
#include <language/error.h>

/**
 * @brief 数字与字符串相乘
 * @param astr 指向字符串的迭代器（指针）
 * @param bint 指向数字的迭代器（指针）
 */
#define int_mul_str(astr, bint)                                                \
    do {                                                                       \
        auto anode = (node_base_string_without_sons*)astr;                     \
        auto bnode = (node_base_int_without_sons*)bint;                        \
        size_t len = strlen(anode->value);                                     \
        anode->value = (char*)realloc(anode->value, len * bnode->value + 1);   \
        if (anode->value == nullptr) {                                         \
            compiler_data.error.send_error_module(                             \
                error::MemoryError, language::error::memoryerror);             \
        }                                                                      \
        for (int j = 1; j <= bnode->value; ++j) {                              \
            strncpy(anode->value + len * j, anode->value, len);                \
        }                                                                      \
        anode->value[len * bnode->value] = '\0';                               \
    } while (0)

#define add_label(a, b)                                                        \
    case token_ticks::ADD: {                                                   \
        a->value += b->value;                                                  \
        break;                                                                 \
    }

#define sub_label(a, b)                                                        \
    case token_ticks::SUB: {                                                   \
        a->value -= b->value;                                                  \
        break;                                                                 \
    }

#define mul_label(a, b)                                                        \
    case token_ticks::MUL: {                                                   \
        a->value *= b->value;                                                  \
        break;                                                                 \
    }

#define equal_label(a, b, receive)                                             \
    case token_ticks::EQUAL: {                                                 \
        auto tmp = new node_base_int_without_sons(a->value == b->value);       \
        delete a;                                                              \
        receive = tmp;                                                         \
        break;                                                                 \
    }

#define unequal_label(a, b, receive)                                           \
    case token_ticks::UNEQUAL: {                                               \
        auto tmp = new node_base_int_without_sons(a->value != b->value);       \
        delete a;                                                              \
        receive = tmp;                                                         \
        break;                                                                 \
    }

namespace trc::compiler {
// todo:加上长整型和长浮点型的常量折叠,转换等功能
treenode* grammar_lex::optimize_expr(is_not_end_node* expr) {
    // 方法很简单，首先模拟执行过程，如果都是同种数据就进行运算压栈，不同种数据或者有变量就原封不动放入栈，一边在栈中模拟运算一边修改节点值
    std::list<decltype(expr->son)::value_type> cal_struct;
    while (!expr->son.empty()) {
        // 运算符
        auto i = expr->son.front();
        expr->son.pop_front();
        if (i->type == grammar_type::OPCODE) {
            // 获取变量值
            if (cal_struct.size() < 2) {
                compiler_data.error.send_error_module(
                    error::SyntaxError, language::error::syntaxerror);
            }
            auto b = cal_struct.back();
            cal_struct.pop_back();
            auto a = cal_struct.back();
            cal_struct.pop_back();
            auto t1 = a->type, t2 = b->type;
            if (!is_no_var_data_node(t1) || !is_no_var_data_node(t2)) {
                // 有一个是变量就无法优化，todo:通过上文进行值推测进行变量优化
                // todo:对于也要变量进行检查，检查是否支持该运算符操作等
                cal_struct.push_back(a);
                cal_struct.push_back(b);
                cal_struct.push_back(i);
                continue;
            }
            token_ticks operator_ = ((node_base_tick_without_sons*)i)->tick;
            delete i;
            // 多种情况判断优化
            // 当类型相等可以直接优化
            // 优化执行过程，释放b点，以a点作为新的节点，避免多余的分配和释放
            if (t1 == t2) {
                switch (t1) {
                case grammar_type::NUMBER: {
                    auto anode = (node_base_int_without_sons*)a,
                         bnode = (node_base_int_without_sons*)b;
                    switch (operator_) {
                    add_label(anode, bnode) sub_label(anode, bnode)
                        mul_label(anode, bnode) case token_ticks::DIV : {
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
                        a = anode_float;
                        anode_float->value /= bnode->value;
                        break;
                    }
                        equal_label(anode, bnode, a)
                            unequal_label(anode, bnode, a) default : {
                            NOREACH("Optimizer met an unexpected cal token %d",
                                (int)(operator_));
                        }
                    }
                    break;
                }
                case grammar_type::STRING: {
                    auto anode = (node_base_string_without_sons*)a,
                         bnode = (node_base_string_without_sons*)b;
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
                            OPERERROR_MSG(t1, t2, i));
                    }
                    }
                    break;
                }
                case grammar_type::FLOAT: {
                    auto anode = (node_base_float_without_sons*)a,
                         bnode = (node_base_float_without_sons*)b;
                    switch (operator_) {
                    add_label(anode, bnode) sub_label(anode, bnode)
                        mul_label(anode, bnode) case token_ticks::DIV : {
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
                cal_struct.push_back(a);
                // 删除b节点
                delete b;
            } else if (t1 == grammar_type::FLOAT
                && t2 == grammar_type::NUMBER) {
                auto anode = (node_base_float_without_sons*)a;
                auto bnode = (node_base_int_without_sons*)b;
                switch (operator_) {
                add_label(anode, bnode) sub_label(anode, bnode)
                    mul_label(anode, bnode) case token_ticks::DIV : {
                    // anode是浮点数，直接相除
                    // 判空
                    if (bnode->value == 0) {
                        compiler_data.error.send_error_module(
                            error::ZeroDivError, language::error::zerodiverror,
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
                delete b;
                cal_struct.push_back(a);
            } else if (t1 == grammar_type::NUMBER
                && t2 == grammar_type::FLOAT) {
                auto anode = (node_base_int_without_sons*)a;
                auto bnode = (node_base_float_without_sons*)b;
                // 保留b节点删除a节点
                switch (operator_) {
                add_label(bnode, anode) case token_ticks::SUB : {
                    bnode->value -= anode->value;
                    bnode->value = -bnode->value;
                    break;
                }
                mul_label(bnode, anode) case token_ticks::DIV : {
                    if (bnode->value == 0) {
                        compiler_data.error.send_error_module(
                            error::ZeroDivError, language::error::zerodiverror,
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
                delete a;
                cal_struct.push_back(b);
            } else if (t1 == grammar_type::STRING && t2 == grammar_type::NUMBER
                && operator_ == token_ticks::MUL) {
                int_mul_str(a, b);
                delete b;
                cal_struct.push_back(a);
            } else if (t1 == grammar_type::NUMBER && t2 == grammar_type::STRING
                && operator_ == token_ticks::MUL) {
                int_mul_str(b, a);
                delete a;
                cal_struct.push_back(b);
            } else {
                // 类型不符合，报错
                compiler_data.error.send_error_module(OPERERROR_MSG(t1, t2, i));
            }
        } else {
            // 数据，直接压入栈
            cal_struct.push_back(i);
        }
    }
    // 如果栈里只剩下一项元素，可以把这一项元素代替掉expr
    if (cal_struct.size() == 1) {
        delete expr;
        return cal_struct.front();
    }
    // 将旧的表达式替换为新的表达式
    expr->son = std::move(cal_struct);
    return nullptr;
}
}
