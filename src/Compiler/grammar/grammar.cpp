#include <Compiler/Compiler.h>
#include <Compiler/grammar.h>
#include <Compiler/pri_compiler.hpp>
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
/**
 * @brief
 * 关于对象的代码转换为节点以及对象信息保存和识别
 * @details
 * 其实就相当于编译时的解释器，负责解析代码并把变量的作用域和类型做出标识，用于各种判断
 */
class grammar_data_control {
public:
    /**
     * @brief 编译从map或者从array中取值的代码
     * @param head 头节点
     * @param code 读取对象代码
     */
    void compile_get_value(treenode* head, const vecs& code);

    /**
     * @brief 编译从map或者从array中创建对象的代码
     * @param head 根节点指针
     * @param code 创建对象代码
     */
    void compile_create_obj(treenode* head, const vecs& code);

    ~grammar_data_control();

private:
    // 保存数组的vector
    vecs array_list;

    // 保存map的vector，用于转成符号表索引
    vecs map_list;

    // 由于map和array获取值的方式相同但是生成的字节码不同，需要在定义时予以区分

    /**
     * @brief 生成从数组中获取值的节点
     */
    void array_get_value(treenode* head);

    /**
     * @brief 生成创建数组的节点
     */
    void create_array(treenode* head, const vecs& code);

    /**
     * @brief 生成从map中获取值的节点
     */
    void map_get_value(treenode* head);

    /**
     * @brief 生成创建map的节点
     */
    void create_map(treenode* head, const vecs& code);
};

void grammar_data_control::array_get_value(treenode* head) {
}

void grammar_data_control::create_array(treenode* head, const vecs& code) {
}

void grammar_data_control::map_get_value(treenode* head) {
}

void grammar_data_control::create_map(treenode* head, const vecs& code) {
}

void grammar_data_control::compile_get_value(treenode* head, const vecs& code) {
}

void grammar_data_control::compile_create_obj(
    treenode* head, const vecs& code) {
}

grammar_data_control::~grammar_data_control() {
    array_list.clear();
    map_list.clear();
}

grammar_lex::~grammar_lex() {
    delete env;
}

void grammar_lex::assign(is_not_end_node* head, trc::compiler::token_ticks oper,
    const code_type& code) {
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
    // 父节点留下子节点记录
    head->connect(ass);
    // 保存变量名
    ass->connect(name);
    // 存放等号右边的值
    ass->connect(get_node());
}

treenode* grammar_lex::callfunction(const code_type& funcname) {
    auto argv_node = new is_not_end_node;
    // 这一段是在切割参数，划分好自己的参数
    token* lex_tmp;
    for (;;) {
        lex_tmp = token_.get_token();
        if (lex_tmp->tick == token_ticks::RIGHT_SMALL_BRACE) {
            delete lex_tmp;
            break;
        } else if (lex_tmp->tick != trc::compiler::token_ticks::COMMA) {
            token_.unget_token(lex_tmp);
            argv_node->connect(get_node());
        } else {
            //逗号
            delete lex_tmp;
        }
    }
    // 由于栈先进后出的特征，在此处将参数进行反转
    std::reverse(argv_node->son.begin(), argv_node->son.end());

    auto* len = new node_base_int_without_sons(argv_node->son.size());

    // 函数名问题：判断内置函数和自定义函数
    if (funcname.size() == 1) {
        if (trc::utils::check_in(funcname[0]->data,
                loader::num_func.begin() + 1, loader::num_func.end())) {
            // 内置函数
            auto builtin
                = new node_base_int(loader::func_num[funcname[0]->data],
                    grammar_type::BUILTIN_FUNC);
            builtin->connect(argv_node);
            builtin->connect(len);
            return builtin;
        } else {
            // 自定义函数
            auto user_defined = new node_base_data(
                grammar_type::CALL_FUNC, funcname[0]->data);

            user_defined->connect(argv_node);
            user_defined->connect(len);
            return user_defined;
        }
    }
}

void grammar_lex::sentence_tree(
    node_base_tick* head, token_ticks sentence_name) {
    size_t argc = 0;

    token* now = token_.get_token();
    if (is_end_token(now->tick)) {
        head->type = grammar_type::OPCODE;
        head->tick = sentence_name;
        delete now;
        return;
    }
    token_.unget_token(now);

    auto* argv_node = new is_not_end_node;

    if (is_sentence_with_one_argv(sentence_name)) {
        head->type = grammar_type::OPCODE_ARGV;
        auto* argv = get_node();
        head->connect(argv);
        return;
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
}

void grammar_lex::while_loop_tree(is_not_end_node* head) {
    auto while_argv = new node_base_tick(grammar_type::OPCODE_ARGV, token_ticks::WHILE);
    // 条件表达式
    head->connect(get_node());
    head->connect(while_argv);
    token* token_data;
    size_t while_start_line = error_->line;
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
    auto data_node = new node_base_int_without_sons(error_->line + 1);
    while_argv->connect(data_node);

    auto* goto_while_line = new node_base_tick(grammar_type::OPCODE_ARGV, token_ticks::GOTO);
    auto line_node = new node_base_int_without_sons(while_start_line);
    goto_while_line->connect(line_node);
    head->connect(goto_while_line);
}

void grammar_lex::if_tree(is_not_end_node* head) {
    auto* if_with_argv = new node_base_tick(grammar_type::OPCODE_ARGV, token_ticks::IF);
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
    auto* data_node = new node_base_int_without_sons(error_->line);
    if_with_argv->connect(data_node);
}

void grammar_lex::func_define(is_not_end_node* head) {
    auto name = check_excepted(token_ticks::NAME);
    auto* func_node
        = new node_base_data(grammar_type::FUNC_DEFINE, name->data);
    delete name;
    auto* line_node = new node_base_data_without_sons(grammar_type::DATA, "");

    func_node->connect(line_node);
    head->connect(func_node);
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
        data_node
            = new node_base_data_without_sons(grammar_type::VAR_NAME, got_tokens[0]->data);
    } else if (is_const_value(tmp)) {
        // 在此将常量转换成数字
        data_node
            = new node_base_int_without_sons(change_const[got_tokens[0]->data]);
    } else if (tmp == token_ticks::LONG_INT_VALUE) {
        data_node
            = new node_base_data_without_sons(grammar_type::LONG_INT, got_tokens[0]->data);
    } else if (tmp == token_ticks::LONG_FLOAT_VALUE) {
        data_node
            = new node_base_data_without_sons(grammar_type::LONG_FLOAT, got_tokens[0]->data);
    } else {
        // 不可能执行到别的代码，执行到就是出bug了
        NOREACH;
    }
    return data_node;
}

treenode* grammar_lex::get_node() {
    trc::compiler::token* now;
    do {
        now = token_.get_token();
        if (now->tick == token_ticks::END_OF_LINE) {
            delete now;
            continue;
        } else {
            break;
        }
    } while (1);
    if (now->tick == trc::compiler::token_ticks::END_OF_TOKENS) {
        delete now;
        return nullptr;
    }
    if (is_cal_value(now->tick)) {
        /*是一个可运算符号*/
        std::vector<token*> got_tokens;
        while (true) {
            got_tokens.push_back(now);
            now = token_.get_token();
            if (is_end_token(now->tick)) {
                /*匹配到结尾还没有匹配到，一定是数据*/
                delete now;
                return make_data_node(got_tokens);
            } else if (now->tick == token_ticks::RIGHT_SMALL_BRACE) {
                token_.unget_token(now);
                return make_data_node(got_tokens);
            }
            if (is_as_token(now->tick)) {
                /*赋值语句*/
                auto head = new is_not_end_node;
                assign(head, now->tick, got_tokens);
                delete now;
                return head;
            } else if (is_cal_token(now->tick) || is_condit_token(now->tick)) {
                /*表达式*/
                auto head = new is_not_end_node;
                got_tokens.push_back(now);
                change_to_last_expr(head, got_tokens);
                return head;
            } else if (now->tick == token_ticks::LEFT_SMALL_BRACE) {
                /*函数调用*/
                delete now;
                return callfunction(got_tokens);
            }
        }
    }
    if (is_sentence_token(now->tick)) {
        // 语句
        auto head = new node_base_tick(grammar_type::TREE, now->tick);
        sentence_tree(head, now->tick);
        delete now;
        return head;
    }
    if (is_blocked_token(now->tick)) {
        auto head = new is_not_end_node;
        if (now->tick == token_ticks::WHILE) {
            while_loop_tree(head);
        } else if (now->tick == token_ticks::IF) {
            if_tree(head);
        } else if (now->tick == token_ticks::FUNC) {
            func_define(head);
        }
        delete now;
        return head;
    }
    error_->send_error_module(error::SyntaxError, language::error::syntaxerror);
    return nullptr;
}

grammar_lex::grammar_lex(
    const std::string& codes_str, trc::compiler::compiler_error* error_)
    : error_(error_)
    , env(new trc::compiler::grammar_data_control)
    , token_(codes_str, error_) {
}

void grammar_lex::ConvertDataToExpressions(token* raw_lex,
    std::vector<treenode*>& st, std::stack<token_ticks>& oper_tmp) {
    token_ticks quicktmp;
    auto tick = raw_lex->tick;
    if (is_cal_value(tick)) {
        token_.unget_token(raw_lex);
        st.push_back(get_node());
        return;
    }
    if (tick == token_ticks::RIGHT_SMALL_BRACE) {
        while ((quicktmp = oper_tmp.top()) != token_ticks::LEFT_SMALL_BRACE) {
            st.push_back(new node_base_tick(grammar_type::OPCODE, quicktmp));
            oper_tmp.pop();
        }
        oper_tmp.pop();
        return;
    }
    if (tick != token_ticks::LEFT_SMALL_BRACE) {
        int quickorder;
        while (!oper_tmp.empty()
            && (quickorder = cal_priority[(quicktmp = oper_tmp.top())]) != '('
            && cal_priority[tick] <= quickorder) {
            st.push_back(new node_base_tick(grammar_type::OPCODE, quicktmp));
            oper_tmp.pop();
        }
    }
    oper_tmp.push(raw_lex->tick);
}

void grammar_lex::change_to_last_expr(is_not_end_node* head, code_type& code) {
    std::vector<treenode*> st;
    is_not_end_node* expr_node = new node_base_data;
    std::stack<token_ticks> oper_tmp;
    for (auto i : code) {
        ConvertDataToExpressions(i, expr_node->son, oper_tmp);
    }
    memory::free_array_obj(code);
    for (;;) {
        trc::compiler::token* raw_lex = token_.get_token();
        if (is_end_token(raw_lex->tick)) {
            delete raw_lex;
            break;
        }
        ConvertDataToExpressions(raw_lex, expr_node->son, oper_tmp);
        delete raw_lex;
    }
    while (!oper_tmp.empty()) {
        token_ticks tmp = oper_tmp.top();
        expr_node->son.push_back(new node_base_tick(grammar_type::OPCODE, tmp));
        oper_tmp.pop();
    }
    head->connect(expr_node);
}

token* grammar_lex::check_excepted(token_ticks tick) {
    token* res = token_.get_token();
    if (res->tick != tick) {
        auto out_error_msg = res->data;
        delete res;
        error_->send_error_module(error::SyntaxError,
            language::error::syntaxerror_no_expect, out_error_msg.c_str());
        return nullptr;
    } else {
        return res;
    }
}
}
