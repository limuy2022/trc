﻿#include <Compiler/Compiler.h>
#include <Compiler/grammar.h>
#include <Compiler/pri_compiler.hpp>
#include <array>
#include <base/Error.h>
#include <base/func_loader.h>
#include <base/utils/data.hpp>
#include <language/error.h>
#include <stack>
#include <string>

#define LINE_NUM compiler_data.error.line

namespace trc::compiler {
treenode* grammar_lex::assign(grammar_type oper, treenode* left_value) {
    // 申请新的树节点
    auto ass = new is_not_end_node(oper, LINE_NUM);
    // 保存变量名
    ass->connect(left_value);
    // 存放等号右边的值
    ass->connect(get_node());
    return ass;
}

void grammar_lex::get_param_list(is_not_end_node* root) {
    // 删除(符号
    delete token_.get_token();
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
            root->connect(get_node());
        }
    }
    special_tick_for_end = token_ticks::UNKNOWN;
}

treenode* grammar_lex::callfunction(token* funcname) {
    // 加上参数个数
    compiler_data.int_size++;
    // 函数名问题：判断内置函数和自定义函数
    is_not_end_node* ans;
    if (trc::utils::str_check_in(
            funcname->data, loader::num_func.begin(), loader::num_func.end())) {
        // 内置函数
        ans = new node_base_int(loader::func_num[funcname->data],
            grammar_type::BUILTIN_FUNC, LINE_NUM);
    } else {
        // 自定义函数
        ans = new node_base_data(grammar_type::CALL_FUNC, funcname, LINE_NUM);
    }
    // 处理参数
    get_param_list(ans);
    // 由于栈先进后出的特征，在此处将参数进行反转
    ans->son.reverse();
    return ans;
}

treenode* grammar_lex::sentence_tree(token_ticks sentence_name) {
    auto head = new node_base_tick(grammar_type::TREE, sentence_name, LINE_NUM);
    int argc = 0;

    token* now = token_.get_token();
    if (is_end_token(now->tick)) {
        head->type = grammar_type::OPCODE;
        head->tick = sentence_name;
        delete now;
        return head;
    }
    token_.unget_token(now);

    auto* argv_node = new is_not_end_node(LINE_NUM);

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
    auto len = new node_base_int_without_sons(argc, LINE_NUM);
    head->connect(argv_node);
    head->connect(len);
    return head;
}

void grammar_lex::read_block(is_not_end_node* root) {
    // 检查并删除下一个大括号
    delete check_excepted(token_ticks::LEFT_BIG_BRACE);
    for (;;) {
        token* token_data = clear_enter();
        if (token_data->tick == token_ticks::RIGHT_BIG_BRACE) {
            delete token_data;
            break;
        }
        token_.unget_token(token_data);
        root->connect(get_node());
    }
}

treenode* grammar_lex::while_if_tree(grammar_type compile_type) {
    auto head = new is_not_end_node(compile_type, LINE_NUM);
    // 条件表达式
    // 设置遇到左大括号停止
    special_tick_for_end = token_ticks::LEFT_BIG_BRACE;
    head->connect(get_node());
    special_tick_for_end = token_ticks::UNKNOWN;
    read_block(head);
    return head;
}

treenode* grammar_lex::func_define() {
    auto name = check_excepted(token_ticks::NAME);
    auto func_node
        = new node_base_data(grammar_type::FUNC_DEFINE, name, LINE_NUM);
    delete name;
    // 处理参数列表
    auto* argv_list = new is_not_end_node(LINE_NUM);
    get_param_list(argv_list);
    func_node->connect(argv_list);
    read_block(func_node);
    return func_node;
}

treenode* grammar_lex::make_data_node(token* data_token) {
    auto tmp = data_token->tick;
    if (tmp == token_ticks::INT_VALUE) {
        return new node_base_int_without_sons(atoi(data_token->data), LINE_NUM);
    } else if (tmp == token_ticks::FLOAT_VALUE) {
        return new node_base_float_without_sons(
            atof(data_token->data), LINE_NUM);
    } else if (tmp == token_ticks::STRING_VALUE) {
        return new node_base_string_without_sons(
            grammar_type::STRING, data_token, LINE_NUM);
    } else if (is_const_value(tmp)) {
        // 在此将常量转换成数字
        compiler_data.int_size++;
        return new node_base_int_without_sons(
            change_const[data_token->data], LINE_NUM);
    } else if (tmp == token_ticks::LONG_INT_VALUE) {
        return new node_base_string_without_sons(
            grammar_type::LONG_INT, data_token, LINE_NUM);
    } else if (tmp == token_ticks::LONG_FLOAT_VALUE) {
        return new node_base_string_without_sons(
            grammar_type::LONG_FLOAT, data_token, LINE_NUM);
    }
    NOREACH("Unexpected data token type %d", (int)tmp);
    return nullptr;
}

token* grammar_lex::clear_enter() {
    token* now;
    do {
        now = token_.get_token();
        if (now->tick == token_ticks::END_OF_LINE) {
            delete now;
            continue;
        }
        return now;
    } while (true);
}

treenode* grammar_lex::get_node(bool end_with_oper) {
    token* now = clear_enter();
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
            }
            // 先前已经生成了节点，直接调用
            return change_to_last_expr(now_node);
        } else if (now->tick == token_ticks::LEFT_SMALL_BRACE) {
            // 遇到左半边小括号，进行表达式处理
            now_node = change_to_last_expr(nullptr);
            // 删除左括号
            delete now;
            // 删除右括号
            delete token_.get_token();
        } else if (nexttick = get_next_token_tick();
                   now->tick == token_ticks::NAME) {
            // 注：有token回退情况的请放在此分支之前
            if (nexttick != token_ticks::LEFT_SMALL_BRACE) {
                // 是标识符且不为函数调用
                now_node = new node_base_string_without_sons(
                    grammar_type::VAR_NAME, now, LINE_NUM);
            } else {
                // 函数调用
                // warning:该地如果直接返回就会造成类似input()+input()这样的解析错误
                // 所以我们要暂存运算结果
                now_node = callfunction(now);
            }
            delete now;
        } else if (now->tick == token_ticks::STORE) {
            // 定义语句
            treenode* res = assign(grammar_type::VAR_DEFINE, now_node);
            delete now;
            return res;
        } else if (now->tick == token_ticks::ASSIGN) {
            // 赋值语句
            treenode* res = assign(grammar_type::VAR_ASSIGN, now_node);
            delete now;
            return res;
        } else if (is_sentence_token(now->tick)) {
            // 语句
            auto res = sentence_tree(now->tick);
            delete now;
            return res;
        } else if (is_blocked_token(now->tick)) {
            token_ticks block_type = now->tick;
            delete now;
            switch (block_type) {
            case token_ticks::WHILE: {
                return while_if_tree(grammar_type::WHILE_BLOCK);
            }
            case token_ticks::IF: {
                return while_if_tree(grammar_type::IF_BLOCK);
            }
            case token_ticks::FUNC: {
                return func_define();
            }
            default: {
                NOREACH("Unexpected grammar block token %d", (int)(block_type));
                return nullptr;
            }
            }
        } else {
            compiler_data.error.send_error_module(
                { error::SyntaxError }, language::error::syntaxerror);
        }
        now = token_.get_token();
    }
    token_.unget_token(now);
    return now_node;
}

grammar_lex::grammar_lex(
    const std::string& codes_str, compiler_public_data& compiler_data)
    : compiler_data(compiler_data)
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
        correct_braces--;
        if (correct_braces == 0) {
            // 代表解析结束，因为遇到了不属于自己的括号
            return;
        }
        while ((quicktmp = oper_stack.top()) != token_ticks::LEFT_SMALL_BRACE) {
            st.push_back(new node_base_tick_without_sons(
                grammar_type::OPCODE, quicktmp, LINE_NUM));
            oper_stack.pop();
        }
        oper_stack.pop();
        return;
    }
    if (tick != token_ticks::LEFT_SMALL_BRACE) {
        while (!oper_stack.empty()
            && (quicktmp = oper_stack.top()) != token_ticks::LEFT_SMALL_BRACE
            && cal_priority[tick] <= cal_priority[quicktmp]) {
            st.push_back(new node_base_tick_without_sons(
                grammar_type::OPCODE, quicktmp, LINE_NUM));
            oper_stack.pop();
        }
    } else {
        correct_braces++;
    }
    oper_stack.push(raw_lex->tick);
    delete raw_lex;
}

#undef OPERERROR

treenode* grammar_lex::change_to_last_expr(treenode* first_data_node) {
    auto head = new is_not_end_node(grammar_type::EXPR, LINE_NUM);
    std::stack<token_ticks> oper_stack;
    if (first_data_node != nullptr) {
        head->son.push_back(first_data_node);
    }
    // 防止吞了别的语句的括号如函数,记录括号对数
    int correct_braces = 1;
    for (;;) {
        trc::compiler::token* raw_lex = token_.get_token();
        if (is_end_token(raw_lex->tick)) {
            delete raw_lex;
            break;
        } else if (raw_lex->tick == special_tick_for_end) {
            token_.unget_token(raw_lex);
            break;
        }
        ConvertDataToExpressions(
            raw_lex, head->son, oper_stack, correct_braces);
        if (correct_braces == 0) {
            // 0作为特殊含义表达，意思是解析到了尽头，所以将token退还确保代码正常
            token_.unget_token(raw_lex);
            break;
        }
    }
    // 无需测试括号是否正确匹配上，这是token_lex的职责
    while (!oper_stack.empty()) {
        token_ticks tmp = oper_stack.top();
        head->son.push_back(new node_base_tick_without_sons(
            grammar_type::OPCODE, tmp, LINE_NUM));
        oper_stack.pop();
    }
    // 此处已经成功且正确生成后缀表达式
    // 进行常量折叠(折叠时会顺便转换类型,修正表达式节点)，也会检查数据类型
    if (compiler_data.option.const_fold) {
        treenode* res;
        if ((res = optimize_expr(head)) != nullptr) {
            // 如果被折叠成一个元素，就会返回一个全新的节点，原先的节点会被释放
            return res;
        }
    }
    return head;
}

token* grammar_lex::check_excepted(token_ticks tick) {
    token* res = token_.get_token();
    if (res->tick != tick) {
        delete res;
        compiler_data.error.send_error_module(
            { error::SyntaxError }, language::error::syntaxerror);
        // 用于消除warning
        return nullptr;
    }
    return res;
}

token_ticks grammar_lex::get_next_token_tick() {
    token* next_tmp = token_.get_token();
    auto nexttick = next_tmp->tick;
    token_.unget_token(next_tmp);
    return nexttick;
}

treenode* grammar_lex::compile_all() {
    auto* root = new is_not_end_node(grammar_type::TREE, LINE_NUM);
    for (treenode* node = get_node(); node != nullptr; node = get_node()) {
        root->connect(node);
    }
    return root;
}
}
