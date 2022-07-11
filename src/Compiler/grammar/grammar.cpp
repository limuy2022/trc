﻿#include <Compiler/Compiler.h>
#include <Compiler/grammar.h>
#include <Compiler/grammar_env.h>
#include <Compiler/optimize.h>
#include <Compiler/pri_compiler.hpp>
#include <Compiler/token.h>
#include <array>
#include <base/Error.h>
#include <base/func_loader.h>
#include <base/memory/memory.hpp>
#include <base/trcdef.h>
#include <base/utils/data.hpp>
#include <language/error.h>
#include <stack>
#include <string>
#include <vector>

namespace trc::compiler {
grammar_lex::~grammar_lex() {
    delete env;
}

treenode* grammar_lex::assign(
    trc::compiler::token_ticks oper, const code_type& code) {
    // 申请新的树节点
    auto ass
        = new trc::compiler::node_base_tick(grammar_type::VAR_DEFINE, oper);
    treenode* name;
    if (code.size() == 1) {
        // 数组之类的名字比一个长
        name = new trc::compiler::node_base_string_without_sons(
            code.front()->data);
    } else {
        NOREACH;
    }
    // 保存变量名
    ass->connect(name);
    // 存放等号右边的值
    ass->connect(get_node());
    return ass;
}

treenode* grammar_lex::callfunction(const code_type& funcname) {
    auto argv_node = new is_not_end_node;
    // 这一段是在切割参数，划分好自己的参数
    token* lex_tmp;
    for (;;) {
        lex_tmp = token_way->get_token();
        if (lex_tmp->tick == token_ticks::RIGHT_SMALL_BRACE) {
            delete lex_tmp;
            break;
        } else if (lex_tmp->tick != trc::compiler::token_ticks::COMMA) {
            token_way->unget_token(lex_tmp);
            argv_node->connect(get_node());
        } else {
            //逗号
            delete lex_tmp;
        }
    }
    // 由于栈先进后出的特征，在此处将参数进行反转
    std::reverse(argv_node->son.begin(), argv_node->son.end());

    // 函数名问题：判断内置函数和自定义函数
    if (funcname.size() == 1) {
        if (trc::utils::check_in(funcname[0]->data,
                loader::num_func.begin() + 1, loader::num_func.end())) {
            // 内置函数
            auto builtin
                = new node_base_int(loader::func_num[funcname[0]->data],
                    grammar_type::BUILTIN_FUNC);
            builtin->connect(argv_node);
            return builtin;
        } else {
            // 自定义函数
            auto user_defined = new node_base_data(
                grammar_type::CALL_FUNC, funcname[0]->data);

            user_defined->connect(argv_node);
            return user_defined;
        }
    }
}

treenode* grammar_lex::sentence_tree(token_ticks sentence_name) {
    auto head = new node_base_tick(grammar_type::TREE, sentence_name);
    size_t argc = 0;

    token* now = token_way->get_token();
    if (is_end_token(now->tick)) {
        head->type = grammar_type::OPCODE;
        head->tick = sentence_name;
        delete now;
        return head;
    }
    token_way->unget_token(now);

    auto* argv_node = new is_not_end_node;

    if (is_sentence_with_one_argv(sentence_name)) {
        head->type = grammar_type::OPCODE_ARGV;
        auto* argv = get_node();
        head->connect(argv);
        return head;
    }

    while (true) {
        token* tmp_ = token_way->get_token();
        if (is_end_token(tmp_->tick)) {
            delete tmp_;
            break;
        } else if (tmp_->tick != token_ticks::COMMA) {
            delete tmp_;
        } else {
            token_way->unget_token(tmp_);
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
        token_data = token_way->get_token();
        if (token_data->tick == token_ticks::RIGHT_BIG_BRACE) [[unlikely]] {
            delete token_data;
            break;
        } else {
            token_way->unget_token(token_data);
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
        token_data = token_way->get_token();
        if (token_data->tick == token_ticks::RIGHT_BIG_BRACE) [[unlikely]] {
            delete token_data;
            break;
        } else {
            token_way->unget_token(token_data);
            head->connect(get_node());
        }
    }
    auto* data_node = new node_base_int_without_sons(compiler_data.error.line);
    if_with_argv->connect(data_node);
    return head;
}

treenode* grammar_lex::func_define() {
    auto head = new is_not_end_node;
    auto name = check_excepted(token_ticks::NAME);
    auto* func_node = new node_base_data(grammar_type::FUNC_DEFINE, name->data);
    delete name;
    auto* line_node = new node_base_data_without_sons(grammar_type::DATA, "");

    func_node->connect(line_node);
    head->connect(func_node);
    return head;
}

treenode* make_data_node(const code_type& got_tokens) {
    is_end_node* data_node = nullptr;
    auto tmp = got_tokens[0]->tick;
    if (tmp == token_ticks::INT_VALUE) {
        data_node
            = new node_base_int_without_sons(atoi(got_tokens[0]->data.c_str()));
    } else if (tmp == token_ticks::FLOAT_VALUE) {
        data_node = new node_base_float_without_sons(
            atof(got_tokens[0]->data.c_str()));
    } else if (tmp == token_ticks::STRING_VALUE) {
        data_node = new node_base_string_without_sons(got_tokens[0]->data);
    } else if (tmp == token_ticks::NAME) {
        data_node = new node_base_data_without_sons(
            grammar_type::VAR_NAME, got_tokens[0]->data);
    } else if (is_const_value(tmp)) {
        // 在此将常量转换成数字
        data_node
            = new node_base_int_without_sons(change_const[got_tokens[0]->data]);
    } else if (tmp == token_ticks::LONG_INT_VALUE) {
        data_node = new node_base_data_without_sons(
            grammar_type::LONG_INT, got_tokens[0]->data);
    } else if (tmp == token_ticks::LONG_FLOAT_VALUE) {
        data_node = new node_base_data_without_sons(
            grammar_type::LONG_FLOAT, got_tokens[0]->data);
    } else {
        // 不可能执行到别的代码，执行到就是出bug了
        NOREACH;
    }
    return data_node;
}

void grammar_lex::get_token_from_token_lex::reload(token_lex& token_) {
    this->token_ = &token_;
}

void grammar_lex::get_token_from_data::reload(const code_type& data) {
    this->data = &data;
    index = 0;
}

token* grammar_lex::get_token_from_token_lex::get_token() {
    return token_->get_token();
}

token* grammar_lex::get_token_from_data::get_token() {
    if (index < data->size()) {
        token* res = data->operator[](index);
        index++;
        return res;
    } else {
        return new token { token_ticks::END_OF_TOKENS, "" };
    }
}

void grammar_lex::get_token_from_data::unget_token(
    [[maybe_unused]] token* back_token) {
    index--;
}

void grammar_lex::get_token_from_token_lex::unget_token(token* back_token) {
    token_->unget_token(back_token);
}

treenode* grammar_lex::get_node() {
    token_from_token_lex.reload(token_);
    // 用于备份上一个获取token的方式，用于还原
    auto backup_token_way = token_way;
    token_way = &token_from_token_lex;
    auto res = get_node_interal();
    token_way = backup_token_way;
    return res;
}

treenode* grammar_lex::get_node(const code_type& code) {
    token_from_data.reload(code);
    // 用于备份上一个获取token的方式，用于还原
    auto backup_token_way = token_way;
    token_way = &token_from_data;
    auto res = get_node_interal();
    token_way = backup_token_way;
    return res;
}

treenode* grammar_lex::get_node_interal() {
    trc::compiler::token* now;
reget:
    now = token_way->get_token();
    if (now->tick == token_ticks::END_OF_LINE) {
        delete now;
        goto reget;
    }

    if (now->tick == trc::compiler::token_ticks::END_OF_TOKENS) {
        delete now;
        return nullptr;
    }
    if (is_cal_value(now->tick)) {
        /*是一个可运算符号*/
        std::vector<token*> got_tokens;
        while (true) {
            got_tokens.push_back(now);
            now = token_way->get_token();
            if (is_end_token(now->tick)) {
                /*匹配到结尾还没有匹配到，一定是数据*/
                token_way->unget_token(now);
                return make_data_node(got_tokens);
            } else if (now->tick == token_ticks::RIGHT_SMALL_BRACE) {
                token_way->unget_token(now);
                return make_data_node(got_tokens);
            }
            if (is_as_token(now->tick)) {
                /*赋值语句*/
                treenode* res = assign(now->tick, got_tokens);
                delete now;
                memory::free_array_obj(got_tokens);
                return res;
            } else if (is_cal_token(now->tick) || is_condit_token(now->tick)) {
                /*表达式*/
                token_way->unget_token(now);
                return change_to_last_expr(got_tokens);
            } else if (now->tick == token_ticks::LEFT_SMALL_BRACE) {
                /*函数调用*/
                delete now;
                return callfunction(got_tokens);
            }
        }
    }
    if (is_sentence_token(now->tick)) {
        // 语句
        auto res = sentence_tree(now->tick);
        delete now;
        return res;
    }
    if (is_blocked_token(now->tick)) {
        treenode* res;
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
            NOREACH;
        }
        }
        delete now;
        return res;
    }
    compiler_data.error.send_error_module(
        error::SyntaxError, language::error::syntaxerror);
    return nullptr;
}

grammar_lex::grammar_lex(
    const std::string& codes_str, compiler_public_data& compiler_data)
    : compiler_data(compiler_data)
    , env(new trc::compiler::grammar_data_control)
    , token_(codes_str, compiler_data) {
}

void grammar_lex::ConvertDataToExpressions(token* raw_lex,
    decltype(is_not_end_node::son)& st, std::stack<token_ticks>& oper_tmp,
    int& correct_braces) {
    // 运用了很基础的将中缀表达式转为后缀表达式的技术
    token_ticks quicktmp;
    auto tick = raw_lex->tick;
    if (is_cal_value(tick)) {
        token_way->unget_token(raw_lex);
        st.push_back(get_node());
        return;
    }
    if (tick == token_ticks::RIGHT_SMALL_BRACE) {
        // 此处先假设括号是对的
        if (correct_braces <= 0) {
            // 代表解析结束，因为遇到了不属于自己的括号
            correct_braces = INT_MAX;
            return;
        }
        while ((quicktmp = oper_tmp.top()) != token_ticks::LEFT_SMALL_BRACE) {
            st.push_back(new node_base_tick(grammar_type::OPCODE, quicktmp));
            oper_tmp.pop();
        }
        oper_tmp.pop();
        correct_braces--;
        return;
    }
    if (tick != token_ticks::LEFT_SMALL_BRACE) {
        int quickorder;
        while (!oper_tmp.empty()
            && (quicktmp = oper_tmp.top()) != token_ticks::LEFT_SMALL_BRACE
            && cal_priority[tick] <= (quickorder = cal_priority[quicktmp])) {
            st.push_back(new node_base_tick(grammar_type::OPCODE, quicktmp));
            oper_tmp.pop();
        }
    } else {
        correct_braces++;
    }
    oper_tmp.push(raw_lex->tick);
}

void grammar_lex::check_expr(is_not_end_node* root) {
    std::stack<treenode*> check_struct;
    for (auto i = root->son.begin(), n = root->son.end(); i != n; ++i) {
        if ((*i)->type == grammar_type::OPCODE) {
            // 运算符
            treenode* a = check_struct.top();
            check_struct.pop();
            treenode* b = check_struct.top();
            check_struct.pop();
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

treenode* grammar_lex::change_to_last_expr(code_type& code) {
    auto head = new is_not_end_node(grammar_type::EXPR);
    std::stack<token_ticks> oper_tmp;
    head->son.push_back(get_node(code));
    memory::free_array_obj(code);
    // 防止吞了别的语句的括号如函数,记录括号对数
    int correct_braces = 0;
    for (;;) {
        trc::compiler::token* raw_lex = token_way->get_token();
        if (is_end_token(raw_lex->tick)) {
            token_way->unget_token(raw_lex);
            break;
        }
        ConvertDataToExpressions(raw_lex, head->son, oper_tmp, correct_braces);
        if (correct_braces == INT_MAX) {
            // INT_MAX作为一个几乎不可能的括号数，作为特殊含义表达，意思是解析到了尽头，所以将token退还同时清零correct_braces确保代码正常
            correct_braces = 0;
            token_way->unget_token(raw_lex);
            break;
        }
        delete raw_lex;
    }
    // 无需测试括号是否正确匹配上，这是token_lex的职责
    while (!oper_tmp.empty()) {
        token_ticks tmp = oper_tmp.top();
        head->son.push_back(
            new node_base_tick_without_sons(grammar_type::OPCODE, tmp));
        oper_tmp.pop();
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
    token* res = token_way->get_token();
    if (res->tick != tick) {
        auto out_error_msg = res->data;
        delete res;
        compiler_data.error.send_error_module(error::SyntaxError,
            language::error::syntaxerror_no_expect, out_error_msg.c_str());
        return nullptr;
    } else {
        return res;
    }
}
}
