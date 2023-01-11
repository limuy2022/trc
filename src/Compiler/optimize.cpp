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
        auto anode = (node_base_string_without_sons*)(astr);                   \
        auto bnode = (node_base_int_without_sons*)(bint);                      \
        size_t len = strlen(anode->value);                                     \
        anode->value = (char*)realloc(anode->value, len * bnode->value + 1);   \
        if (anode->value == nullptr) {                                         \
            compiler_data.error.send_error_module(                             \
                { error::MemoryError }, language::error::memoryerror);         \
        }                                                                      \
        for (int j = 1; j <= bnode->value; ++j) {                              \
            strncpy(anode->value + len * j, anode->value, len);                \
        }                                                                      \
        anode->value[len * bnode->value] = '\0';                               \
    } while (0)

/**
 * @brief
 * 该函数用于计算整形时代替标准库的pow，因为pow函数使用double进行运算，可能会使结果有偏差
 * @return a的b次方
 * @warning 仅限整数或小数的整数次方
 */
template <typename T> static T fast_pow(T a, T b) {
    if (b == 0) {
        return 1;
    }
    T tmp = fast_pow(a, b / 2);
    tmp *= tmp;
    if (b % 2) {
        tmp *= a;
    }
    return tmp;
}

namespace trc::compiler {
template <typename result_type, typename value_type>
treenode* number_node_cal(value_type avalue, value_type bvalue,
    token_ticks oper, compiler_public_data& compiler_data) {
    switch (oper) {
    case token_ticks::ADD: {
        return new result_type(avalue + bvalue);
    }
    case token_ticks::SUB: {
        return new result_type(avalue - bvalue);
    }
    case token_ticks::MUL: {
        return new result_type(avalue * bvalue);
    }
    case token_ticks::DIV: {
        if (!bvalue) {
            compiler_data.error.send_error_module({ error::ZeroDivError },
                language::error::zerodiverror, std::to_string(avalue).c_str());
        }
        // 除法一定是浮点数
        return new node_base_float_without_sons(avalue * 1.0 / bvalue);
    }
    case token_ticks::MOD: {
        if (!bvalue) {
            compiler_data.error.send_error_module({ error::ZeroDivError },
                language::error::zerodiverror, std::to_string(avalue).c_str());
        }
        if constexpr (std::is_same<result_type,
                          node_base_float_without_sons>::value) {
            // 有一方是浮点数
            return new node_base_float_without_sons(fmod(avalue, bvalue));
        } else {
            return new node_base_int_without_sons(avalue % bvalue);
        }
    }
    case token_ticks::POW: {
        if constexpr (std::is_same<result_type,
                          node_base_float_without_sons>::value) {
            return new node_base_float_without_sons(pow(avalue, bvalue));
        } else {
            return new node_base_int_without_sons(fast_pow(avalue, bvalue));
        }
    }
    case token_ticks::ZDIV: {
        return new node_base_int_without_sons(int(avalue / bvalue));
    }
    case token_ticks::EQUAL: {
        return new node_base_int_without_sons(avalue == bvalue);
    }
    case token_ticks::UNEQUAL: {
        return new node_base_int_without_sons(avalue != bvalue);
    }
    case token_ticks::GREATER: {
        return new node_base_int_without_sons(avalue > bvalue);
    }
    case token_ticks::LESS: {
        return new node_base_int_without_sons(avalue < bvalue);
    }
    case token_ticks::GREATER_EQUAL: {
        return new node_base_int_without_sons(avalue >= bvalue);
    }
    case token_ticks::LESS_EQUAL: {
        return new node_base_int_without_sons(avalue <= bvalue);
    }
    default: {
        return nullptr;
    }
    }
}

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
                    { error::SyntaxError }, language::error::syntaxerror);
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
            if (is_number_class_node(t1) && is_number_class_node(t2)) {
                treenode* res;
                if (t1 == grammar_type::NUMBER && t2 == grammar_type::NUMBER) {
                    res = number_node_cal<node_base_int_without_sons>(
                        ((node_base_int_without_sons*)a)->value,
                        ((node_base_int_without_sons*)b)->value, operator_,
                        compiler_data);
                } else {
                    double avalue = dynamic_cast<number_node*>(a)->to_float();
                    double bvalue = dynamic_cast<number_node*>(b)->to_float();
                    res = number_node_cal<node_base_float_without_sons>(
                        avalue, bvalue, operator_, compiler_data);
                }
                if (res == nullptr) {
                    compiler_data.error.send_error_module(
                        OPERERROR_MSG(t1, t2, i));
                }
                delete a;
                delete b;
                cal_struct.push_back(res);
            } else if (t1 == grammar_type::STRING
                && t2 == grammar_type::STRING) {
                auto anode = (node_base_string_without_sons*)a,
                     bnode = (node_base_string_without_sons*)b;
                switch (operator_) {
                case token_ticks::ADD: {
                    size_t astrlen = strlen(anode->value);
                    anode->value = (char*)realloc(anode->value,
                        sizeof(char) * (astrlen + strlen(bnode->value) + 1));
                    if (anode->value == nullptr) {
                        compiler_data.error.send_error_module(
                            { error::MemoryError },
                            language::error::memoryerror);
                    }
                    strcpy(anode->value + astrlen, bnode->value);
                    break;
                }
                case token_ticks::EQUAL: {
                    auto tmp = new node_base_int_without_sons(
                        !strcmp(anode->value, bnode->value));
                    delete anode;
                    a = tmp;
                    break;
                }
                case token_ticks::UNEQUAL: {
                    auto tmp = new node_base_int_without_sons(
                        strcmp(anode->value, bnode->value));
                    delete anode;
                    a = tmp;
                    break;
                }
                case token_ticks::GREATER: {
                    auto tmp = new node_base_int_without_sons(
                        strcmp(anode->value, bnode->value) > 0);
                    delete anode;
                    a = tmp;
                    break;
                }
                case token_ticks::LESS: {
                    auto tmp = new node_base_int_without_sons(
                        strcmp(anode->value, bnode->value) < 0);
                    delete anode;
                    a = tmp;
                    break;
                }
                case token_ticks::GREATER_EQUAL: {
                    auto res = strcmp(anode->value, bnode->value);
                    delete anode;
                    a = new node_base_int_without_sons(res > 0 || res == 0);
                    break;
                }
                case token_ticks::LESS_EQUAL: {
                    auto res = strcmp(anode->value, bnode->value);
                    delete anode;
                    a = new node_base_int_without_sons(res < 0 || res == 0);
                    break;
                }
                default: {
                    compiler_data.error.send_error_module(
                        OPERERROR_MSG(t1, t2, i));
                }
                }
                delete b;
                cal_struct.push_back(a);
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
