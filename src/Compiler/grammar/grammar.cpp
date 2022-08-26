#include <Compiler/Compiler.h>
#include <Compiler/grammar.h>
#include <Compiler/grammar_env.h>
#include <Compiler/pri_compiler.hpp>
#include <array>
#include <base/Error.h>
#include <base/func_loader.h>
#include <base/trcdef.h>
#include <base/utils/data.hpp>
#include <easyloggingpp/easylogging++.h>
#include <language/error.h>
#include <stack>
#include <string>

namespace trc::compiler {
grammar_lex::~grammar_lex() {
    delete env;
}

treenode* grammar_lex::assign(
    trc::compiler::token_ticks oper, treenode* left_value) {
    // 申请新的树节点
    auto ass
        = new trc::compiler::node_base_tick(grammar_type::VAR_DEFINE, oper);
    // 保存变量名
    ass->connect(left_value);
    // 存放等号右边的值
    ass->connect(get_node());
    return ass;
}

treenode* grammar_lex::callfunction(token* funcname) {
    auto argv_node = new is_not_end_node;
    // 这一段是在切割参数，划分好自己的参数
    token* lex_tmp;
    // 设置解析器遇到逗号停止解析
    special_tick_for_end = token_ticks::COMMA;
    for (;;) {
        lex_tmp = token_.get_token();
        if (lex_tmp->tick == token_ticks::RIGHT_SMALL_BRACE) {
            delete lex_tmp;
            break;
        } else if (lex_tmp->tick == token_ticks::COMMA) {
            delete lex_tmp;
        } else {
            token_.unget_token(lex_tmp);
            argv_node->connect(get_node());
        }
    }
    special_tick_for_end = token_ticks::UNKNOWN;
    // 由于栈先进后出的特征，在此处将参数进行反转
    std::reverse(argv_node->son.begin(), argv_node->son.end());

    // 函数名问题：判断内置函数和自定义函数
    if (trc::utils::str_check_in(
            funcname->data, loader::num_func.begin(), loader::num_func.end())) {
        // 内置函数
        auto builtin = new node_base_int(
            loader::func_num[funcname->data], grammar_type::BUILTIN_FUNC);
        builtin->connect(argv_node);
        return builtin;
    } else {
        // 自定义函数
        auto user_defined
            = new node_base_data(grammar_type::CALL_FUNC, funcname->data);

        user_defined->connect(argv_node);
        return user_defined;
    }
}

treenode* grammar_lex::sentence_tree(token_ticks sentence_name) {
    auto head = new node_base_tick(grammar_type::TREE, sentence_name);
    size_t argc = 0;

    token* now = token_.get_token();
    if (is_end_token(now->tick)) {
        head->type = grammar_type::OPCODE;
        head->tick = sentence_name;
        delete now;
        return head;
    }
    token_.unget_token(now);

    auto* argv_node = new is_not_end_node;

    if (is_sentence_with_one_argv(sentence_name)) {
        head->type = grammar_type::OPCODE_ARGV;
        auto* argv = get_node();
        head->connect(argv);
        return head;
    }

    while (true) {
        token* tmp_ = token_.get_token();
        if (is_end_token(tmp_->tick)) {
            delete tmp_;
            break;
        } else if (tmp_->tick != token_ticks::COMMA) {
            delete tmp_;
        } else {
            token_.unget_token(tmp_);
            argv_node->connect(get_node());
            ++argc;
        }
    }
    // 将参数反转，因为栈先进后出
    std::reverse(argv_node->son.begin(), argv_node->son.end());

    auto len = new node_base_int_without_sons(argc);
    head->connect(argv_node);
    head->connect(len);
    return head;
}

treenode* grammar_lex::while_loop_tree() {
    auto head = new is_not_end_node;
    auto while_argv
        = new node_base_tick(grammar_type::OPCODE_ARGV, token_ticks::WHILE);
    // 条件表达式
    head->connect(get_node());
    head->connect(while_argv);
    token* token_data;
    size_t while_start_line = compiler_data.error.line;
    for (;;) {
        token_data = token_.get_token();
        if (token_data->tick == token_ticks::RIGHT_BIG_BRACE) [[unlikely]] {
            delete token_data;
            break;
        } else {
            token_.unget_token(token_data);
            head->connect(get_node());
        }
    }
    // 如果条件不满足，调到goto语句之后出循环
    auto data_node
        = new node_base_int_without_sons(compiler_data.error.line + 1);
    while_argv->connect(data_node);

    auto* goto_while_line
        = new node_base_tick(grammar_type::OPCODE_ARGV, token_ticks::GOTO);
    auto line_node = new node_base_int_without_sons(while_start_line);
    goto_while_line->connect(line_node);
    head->connect(goto_while_line);
    return head;
}

treenode* grammar_lex::if_tree() {
    auto head = new is_not_end_node;
    auto* if_with_argv
        = new node_base_tick(grammar_type::OPCODE_ARGV, token_ticks::IF);
    // 条件表达式
    head->connect(get_node());
    head->connect(if_with_argv);
    token* token_data;
    for (;;) {
        token_data = token_.get_token();
        if (token_data->tick == token_ticks::RIGHT_BIG_BRACE) [[unlikely]] {
            delete token_data;
            break;
        } else {
            token_.unget_token(token_data);
            head->connect(get_node());
        }
    }
    auto* data_node = new node_base_int_without_sons(compiler_data.error.line);
    if_with_argv->connect(data_node);
    return head;
}

treenode* grammar_lex::func_define() {
    auto head = new is_not_end_node;
    // auto name = check_excepted(token_ticks::NAME);
    // auto* func_node = new node_base_data(grammar_type::FUNC_DEFINE,
    // name->data); delete name; auto* line_node = new
    // node_base_data_without_sons(grammar_type::DATA, "");

    // func_node->connect(line_node);
    // head->connect(func_node);
    return head;
}

/**
 * @brief 根据token制作相应的数据节点
 * @warning 不负责变量节点的生成
 */
treenode* make_data_node(token* data_token) {
    auto tmp = data_token->tick;
    if (tmp == token_ticks::INT_VALUE) {
        return new node_base_int_without_sons(atoi(data_token->data));
    } else if (tmp == token_ticks::FLOAT_VALUE) {
        return new node_base_float_without_sons(atof(data_token->data));
    } else if (tmp == token_ticks::STRING_VALUE) {
        return new node_base_data_without_sons(
            grammar_type::STRING, data_token);
    } else if (is_const_value(tmp)) {
        // 在此将常量转换成数字
        return new node_base_int_without_sons(change_const[data_token->data]);
    } else if (tmp == token_ticks::LONG_INT_VALUE) {
        return new node_base_data_without_sons(
            grammar_type::LONG_INT, data_token);
    } else if (tmp == token_ticks::LONG_FLOAT_VALUE) {
        return new node_base_data_without_sons(
            grammar_type::LONG_FLOAT, data_token);
    } else {
        // 不可能执行到别的代码，执行到就是出bug了
        LOG(FATAL) << "Unexpected data token type " << (int)tmp;
    }
}

treenode* grammar_lex::get_node(bool end_with_oper) {
    trc::compiler::token* now;
reget:
    now = token_.get_token();
    if (now->tick == token_ticks::END_OF_LINE) {
        delete now;
        goto reget;
    }
    if (now->tick == trc::compiler::token_ticks::END_OF_TOKENS) {
        delete now;
        return nullptr;
    }
    treenode* now_node = nullptr;
    token_ticks nexttick;
    while (!is_end_token(now->tick) && now->tick != special_tick_for_end) {
        if (now->tick == token_ticks::RIGHT_SMALL_BRACE) {
            token_.unget_token(now);
            break;
        }
        if (is_novar_data_token(now->tick)) {
            // 在这个区间就是非变量数据节点
            now_node = make_data_node(now);
            delete now;
        } else if (is_cal_token(now->tick) || is_condit_token(now->tick)) {
            /*表达式*/
            token_.unget_token(now);
            if (end_with_oper) {
                return now_node;
            } else {
                // 先前已经生成了节点，直接调用
                return change_to_last_expr(now_node);
            }
        } else if (nexttick = get_next_token_tick();
                   now->tick == token_ticks::NAME) {
            // 注：有token回退情况的请放在此分支之前
            if (nexttick != token_ticks::LEFT_SMALL_BRACE) {
                // 是标识符且不为函数调用
                now_node = new node_base_data_without_sons(
                    grammar_type::VAR_NAME, now);
            } else {
                // 函数调用
                // warning:该地如果直接返回就会造成类似input()+input()这样的解析错误
                // 所以我们要暂存运算结果
                // 删除(符号
                delete token_.get_token();
                now_node = callfunction(now);
            }
            delete now;
        } else if (is_as_token(now->tick)) {
            /*赋值语句*/
            treenode* res = assign(now->tick, now_node);
            delete now;
            return res;
        } else if (is_sentence_token(now->tick)) {
            // 语句
            auto res = sentence_tree(now->tick);
            delete now;
            return res;
        } else if (is_blocked_token(now->tick)) {
            treenode* res = nullptr;
            switch (now->tick) {
            case token_ticks::WHILE: {
                res = while_loop_tree();
                break;
            }
            case token_ticks::IF: {
                res = if_tree();
                break;
            }
            case token_ticks::FUNC: {
                res = func_define();
                break;
            }
            default: {
                LOG(FATAL) << "Unexpected grammar block token "
                           << (int)(now->tick);
            }
            }
            delete now;
            return res;
        } else {
            compiler_data.error.send_error_module(
                error::SyntaxError, language::error::syntaxerror);
        }
        now = token_.get_token();
    }
    token_.unget_token(now);
    return now_node;
}

grammar_lex::grammar_lex(
    const std::string& codes_str, compiler_public_data& compiler_data)
    : compiler_data(compiler_data)
    , env(new trc::compiler::grammar_data_control)
    , token_(codes_str, compiler_data) {
}

void grammar_lex::ConvertDataToExpressions(token* raw_lex,
    decltype(is_not_end_node::son)& st, std::stack<token_ticks>& oper_stack,
    int& correct_braces) {
    // 运用了很基础的将中缀表达式转为后缀表达式的技术
    auto tick = raw_lex->tick;
    if (is_cal_value(tick)) {
        token_.unget_token(raw_lex);
        st.push_back(get_node(true));
        return;
    }
    token_ticks quicktmp;
    if (tick == token_ticks::RIGHT_SMALL_BRACE) {
        // 此处先假设括号是对的
        if (correct_braces <= 0) {
            // 代表解析结束，因为遇到了不属于自己的括号
            correct_braces = -1;
            return;
        }
        while ((quicktmp = oper_stack.top()) != token_ticks::LEFT_SMALL_BRACE) {
            st.push_back(new node_base_tick(grammar_type::OPCODE, quicktmp));
            oper_stack.pop();
        }
        oper_stack.pop();
        correct_braces--;
        return;
    }
    if (tick != token_ticks::LEFT_SMALL_BRACE) {
        while (!oper_stack.empty()
            && (quicktmp = oper_stack.top()) != token_ticks::LEFT_SMALL_BRACE
            && cal_priority[tick] <= cal_priority[quicktmp]) {
            st.push_back(new node_base_tick(grammar_type::OPCODE, quicktmp));
            oper_stack.pop();
        }
    } else {
        correct_braces++;
    }
    oper_stack.push(raw_lex->tick);
    delete raw_lex;
}

void grammar_lex::check_expr(is_not_end_node* root) {
    std::stack<treenode*> check_struct;
    for (auto i = root->son.begin(), n = root->son.end(); i != n; ++i) {
        if ((*i)->type == grammar_type::OPCODE) {
            // 运算符
            treenode* a = pop_oper_stack(check_struct);
            treenode* b = pop_oper_stack(check_struct);
            auto t1 = a->type, t2 = b->type;
            if (t1 == grammar_type::VAR_NAME || t2 == grammar_type::VAR_NAME) {
                // 变量类型不确定，没有检查的意义
                continue;
            }
            if (!(is_no_var_data_node(t1) && is_no_var_data_node(t2))) {
                // 非基础类型，todo:支持类和操作符重载后，非基础类型也要支持表达式检查，记得把优化器的非基础表达式检查加上
                continue;
            }
            // 类型检查比较简单，检查字符型的运算就可以了
            // 数字与字符串只能允许乘法运算
            if (t1 == grammar_type::STRING && is_number_class_node(t2)) {
                if (((node_base_tick_without_sons*)root)->tick
                    != token_ticks::MUL) {
                    compiler_data.error.send_error_module(
                        OPERERROR_MSG(t1, t2, *i));
                }
                if (t1 != grammar_type::NUMBER) {
                    compiler_data.error.send_error_module(
                        OPERERROR_MSG(t1, t2, *i));
                }
            } else if (t2 == grammar_type::STRING && is_number_class_node(t1)) {
                if (((node_base_tick_without_sons*)root)->tick
                    != token_ticks::MUL) {
                    compiler_data.error.send_error_module(
                        OPERERROR_MSG(t1, t2, *i));
                }
                if (t2 != grammar_type::NUMBER) {
                    compiler_data.error.send_error_module(
                        OPERERROR_MSG(t1, t2, *i));
                }
            }
        } else {
            check_struct.push(*i);
        }
    }
}

#undef OPERERROR

treenode* grammar_lex::change_to_last_expr(treenode* first_data_node) {
    auto head = new is_not_end_node(grammar_type::EXPR);
    std::stack<token_ticks> oper_stack;
    head->son.push_back(first_data_node);
    // 防止吞了别的语句的括号如函数,记录括号对数
    int correct_braces = 0;
    for (;;) {
        trc::compiler::token* raw_lex = token_.get_token();
        if (is_end_token(raw_lex->tick)) {
            delete raw_lex;
            break;
        }
        ConvertDataToExpressions(
            raw_lex, head->son, oper_stack, correct_braces);
        if (correct_braces == -1) {
            // -1作为特殊含义表达，意思是解析到了尽头，所以将token退还同时清零correct_braces确保代码正常
            token_.unget_token(raw_lex);
            break;
        }
    }
    // 无需测试括号是否正确匹配上，这是token_lex的职责
    while (!oper_stack.empty()) {
        token_ticks tmp = oper_stack.top();
        head->son.push_back(
            new node_base_tick_without_sons(grammar_type::OPCODE, tmp));
        oper_stack.pop();
    }
    // 此处已经成功且正确生成后缀表达式，在此检查参数判断是否进行常量折叠
    if (compiler_data.option->optimize) {
        // 进行常量折叠(折叠时会顺便转换类型,修正表达式节点)，也会检查数据类型
        optimize_expr(head);
    } else {
        // 不进行优化，就要额外进行类型检查
        check_expr(head);
    }
    return head;
}

token* grammar_lex::check_excepted(token_ticks tick) {
    token* res = token_.get_token();
    if (res->tick != tick) {
        auto out_error_msg = res->data;
        delete res;
        compiler_data.error.send_error_module(error::SyntaxError,
            language::error::syntaxerror_no_expect, out_error_msg);
        return nullptr;
    } else {
        return res;
    }
}

token_ticks grammar_lex::get_next_token_tick() {
    token* next_tmp = token_.get_token();
    auto nexttick = next_tmp->tick;
    token_.unget_token(next_tmp);
    return nexttick;
}
}
