/**
 * 此处是编译器的最重要核心之一
 * 主要负责生成供编译器生成字节码的语法树
 * 
 * 语法树描述规则：
 * tree节点不储存数据
 */

#include <cstring>
#include <string>
#include <vector>
#include "../../include/share.h"
#include "../../include/node.h"
#include "../../include/data.hpp"
#include "../../include/Compiler/optimize.h"
#include "../../include/type.hpp"
#include "../../include/type.h"
#include "../../include/func_loader.h"
#include "../../include/Error.h"
#include "obj.h"

using namespace std;

// 运算符和条件解析
#define OPER_TREE(list_var_, __code_)\
for(const auto &i : (list_var_)) {\
    if(check_in(i, (__code_))) {\
        oper_tree(head, (__code_), i);\
        return head;\
    }\
}

static vecs2d codes_s;
static vector<treenode *> *result;
// 函数标志
static const vecs func_symbol = {"(", ")"};

static treenode *real_grammar(treenode *head, const vecs &code);

static vecs cut(const vector<string> &origin, int start, int end) {
    /**
     *功能性函数，切取start到end的距离，具体规则参考python的切片
     *“切头不切尾”
    */

    const auto& tmp = origin.begin();
    return vecs(tmp + start, tmp + end);
}

static vecs cut(const vector<string> &origin, const string &start, const string &end) {
    /**
     *功能性函数，切取start到end的距离，具体规则参考python的切片
     *“切头不切尾”
    */

    const auto& start_iter = origin.begin(), &end_iter = origin.end();
    return vecs(find(start_iter, end_iter, start), find(start_iter, end_iter, end));
}

static vecs until_cut(const vecs &code, const string &cut_sym) {
    /**
     * 切除直到遇到某个字符串
     */

    const auto &start = code.begin();
    return vecs(start, find(start, code.end(), cut_sym));
}

static vecs from_cut_until(const vecs &code, const string &cut_sym) {
    /**
     * 从某个字符串开始切到末尾
    */

    const auto& end = code.end(), &start = find(code.begin(), end, cut_sym) + 1;
    return vecs(start, end);
}

static treenode *ifis(const vecs &code) {
    /**
     * 本节点是对real_grammar函数的间接递归
     * 当code为数据时返回DATA节点
     * 当code为表达式时返回TREE节点
     * 注：本函数仅仅生成节点，对于指向不作管理
     */

    if (code.size() == 1)
        return new treenode(DATA, code[0]);
    auto *result = new treenode;
    real_grammar(result, code);
    return result;
}

static size_t count_line() {
    /**
     * 计算从{符号开始，到其对应的}符号的结束索引
     */

    int break_num = 1;
    size_t last_index = LINE_NOW + 1;
    for (;;) {
        const vecs &line = codes_s[last_index];

        if (check_in(string("}"), line)) {
            break_num--;
            if (!break_num)
                break;
        } else if (check_in(string("{"), line))
            break_num++;
        ++last_index;
    }
    return last_index + 1;
}

static bool in_func(int index_oper, const vecs &code) {
    /**
     * 判断某个符号是否处于语句中
     * @param index_oper : 符号所在的索引
     * @param code : 符号所在的语句
     */

    size_t break_num = 0, n = code.size(), start_sym, end_sym;
    // 循环次数，其实就是（）的对数，因为要全部遍历完
    size_t loop_num = count(code.begin(), code.end(), "(");
    for (int i = 0; i < n; ++i) {
        if (code[i] == "(") {
            start_sym = i;
            break_num++;
        } else if (code[i] == ")") {
            end_sym = i;
            --break_num;
            if (!break_num) {
                if (!(start_sym < index_oper && end_sym > index_oper))
                    return false;
                --loop_num;
                if (!loop_num) {
                    return true;
                }
            }
        }
    }
    return false;
}

static void assign(treenode *head, const string &oper, const vecs &code) {
    /**
     * 生成赋值语句节点
     */

    // 申请新的树节点
    auto ass = new treenode, \
 name = new treenode, \
 opernode = new treenode(VAR_DEFINE, oper);
    // 父节点指向
    const vecs& temp = until_cut(code, oper);
    if (temp.size() == 1) {
        // 数组之类的名字比一个长
        name->data = temp[0];
        name->type = DATA;
    } else {
    }

    // 新的树节点，存放等号右边的值
    treenode *right_tree = ifis(from_cut_until(code, oper));
    // 父节点留下子节点记录
    head->connect(ass);
    opernode->connect(name);
    ass->connect(right_tree);
    ass->connect(opernode);
}

static void function(treenode *head, const vecs &code) {
    /**
     * 生成函数调用节点
     */

    const auto &name = until_cut(code, "(");
    vecs add_temp;
    int argc = 0;
    auto functree = new treenode, argv_node = new treenode;
    head->connect(functree);
    functree->connect(argv_node);

    const auto &argv = from_cut_until(code, "(");
    size_t argv_n = argv.size(), break_num = 0;
    if (argv_n != 1) {
        for (const auto &i : argv) {
            if (i == "(")
                break_num++;
            if ((i == "," || i == ")") && break_num == 0) {
                argv_node->connect(ifis(add_temp));
                argc++;
                add_temp.clear();
            } else
                add_temp.push_back(i);
            if (i == ")")
                break_num--;
        }
    }

    // 由于栈先进后出的特征，在此处将参数进行反转
    vector<treenode *> finally_temp;
    size_t f_n = argv_node->son.size();
    if (f_n > 1) {
        for (int i = 1; i <= f_n; ++i) {
            finally_temp.push_back(argv_node->son[f_n - i]);
        }
        argv_node->son = finally_temp;
    }

    add_temp.clear();
    auto *len = new treenode(DATA, to_string(argc));
    functree->connect(len);

    // 函数名问题：判断内置函数和自定义函数
    if (name.size() == 1) {
        if (check_in_array(num_func, name[0], BUILTINS_FUNC_NUMS)) {
            // 内置函数
            auto *builtin = new treenode(BUILTIN_FUNC), *bycode = new treenode(DATA), *nodeargv = new treenode(
                    DATA);
            bycode->data = "CALL_BUILTIN";
            nodeargv->data = to_string(func_num[name[0]]);
            builtin->connect(bycode);
            builtin->connect(nodeargv);
            functree->connect(builtin);
        } else {
            auto *builtin = new treenode(CALL_FUNC), *bycode = new treenode(DATA), *nodeargv = new treenode(
                    DATA);
            bycode->data = "CALL_FUNCTION";
            nodeargv->data = to_string(func_num[name[0]]);
            builtin->connect(bycode);
            builtin->connect(nodeargv);
            functree->connect(builtin);
        }
    }
}


static void sentence_tree(treenode *head, const vecs &code) {
    /**
     * 生成语句执行节点
     */

    auto *argv_node = new treenode;
    const string& sentence_name = code[0];
    size_t n = code.size(), argc = 0;
    if (n == 1) {
        // 单个语句
        auto *snode = new treenode(ORIGIN, sentence_name);
        head->connect(snode);
        return;
    } else if (check_in(code[0], sentences_yes_argv)) {
        auto *snode = new treenode(OPCODE_ARGV, code[0]), \
 *argv = new treenode(DATA, code[1]);
        snode->connect(argv);
        head->connect(snode);
        return;
    }
    auto *snode = new treenode;

    const vecs& argv = from_cut_until(code, code[0]);
    vecs add_temp;
    size_t argv_n = argv.size();

    for (int i = 0; i < argv_n; ++i) {
        if (argv[i] == ",") {
            argv_node->connect(ifis(add_temp));
            ++argc;
            add_temp.clear();
        } else
            add_temp.push_back(argv[i]);
    }
    argv_node->connect(ifis(add_temp));
    ++argc;

    auto len = new treenode(DATA, to_string(argc)), name_node = new treenode(OPCODE, sentence_name);
    snode->connect(argv_node);
    snode->connect(len);
    snode->connect(name_node);
    head->connect(snode);
}

static void while_loop_tree(treenode *head, const vecs &code) {
    /**
     * 生成条件循环字节码
     * 注：仅对当前行进行解析
     * 循环通过跳转实现
     */

    const auto& begin_ = code.begin(), \
 &end_ = code.end();
    // 条件表达式
    const vecs& bool_sen = cut(code, *(find(begin_, end_, "while") + 1), "{");
    // 将条件表达式解析为语法树
    size_t line = count_line();
    // 设置循环结束时跳转
    size_t end_line = line - 1;
    auto *opcode_ = new treenode(OPCODE_ARGV, "if"), \
 *data_node = new treenode(DATA, to_string(line));
    opcode_->connect(data_node);
    head->connect(ifis(bool_sen));
    head->connect(opcode_);
    codes_s[end_line] = {"goto", to_string(LINE_NOW)};
}

static void if_tree(treenode *head, const vecs &code) {
    /**
     * 生成条件判断字节码
     * 注：仅对当前行进行解析
     * 判断通过跳转实现
     */

    const auto &begin_ = code.begin(), \
 &end_ = code.end();
    // 条件表达式
    const vecs& bool_sen = cut(code, *(find(begin_, end_, "if") + 1), "{");
    // 将条件表达式解析为语法树
    size_t line = count_line();
    auto *opcode_ = new treenode(OPCODE_ARGV, "if"), \
 *data_node = new treenode(DATA, to_string(line));
    opcode_->connect(data_node);
    head->connect(ifis(bool_sen));
    head->connect(opcode_);
}

static void func_define(treenode *head, const vecs &code) {
    /**
     *定义函数，原理：标记出函数结束的位置，使字节码生成器把函数单独解析
     * 使用goto语句改变定义
     */

    auto *func_node = new treenode(FUNC_DEFINE, code[1]), \
 *line_node = new treenode(DATA, to_string(count_line()));

    func_node->connect(line_node);
    head->connect(func_node);
}

static bool block(treenode *head, const vecs &code) {
    /**
     * 此函数负责所有关于语句块的语句的解析
     * 包括func，if， while等
     */

    const char * name = code[0].c_str();
    if (!strcmp(name, "if")) {
        if_tree(head, code);
        return true;
    }
    if (!strcmp(name, "while")) {
        while_loop_tree(head, code);
        return true;
    }
    if (!strcmp(name, "func")) {
        func_define(head, code);
        return true;
    }
    return false;
}

static void oper_tree(treenode *head, const vecs &code, const string &oper) {
    /**
     * 运算符和条件运算符解析，+-*%,==,!=等运算符
     */

    auto oper_node = new treenode(TREE), \
        oper_data = new treenode(OPCODE, oper),\
        left = ifis(until_cut(code, oper)), \
        right = ifis(from_cut_until(code, oper));

    if (left->type == DATA && \
            right->type == DATA && \
            what_type(left->data) == INT_TICK && \
            what_type(right->data) == INT_TICK
            ) {
        auto *result_oper = new treenode(DATA);
        // 仅允许整型进行运算符的常量折叠
        if (check_in(oper, opti_condits)) {
            // 布尔值
            result_oper->data = to_string(
                    optimize_condit[oper](to_type<int>(left->data), to_type<int>(right->data)));
            head->connect(result_oper);
            return;
        } else if (check_in(oper, opti_opers)) {
            result_oper->data = to_string(
                    optimize_number[oper](to_type<int>(left->data), to_type<int>(right->data)));
            head->connect(result_oper);
            return;
        }
    }
    oper_node->connect(left);
    oper_node->connect(right);
    oper_node->connect(oper_data);
    head->connect(oper_node);
}

static void not_tree(treenode *head, const vecs &code) {
    /**
     * !非运算符
     */

    head->connect(ifis(from_cut_until(code, "not")));
    head->connect(new treenode(OPCODE, "not"));
}

static treenode *real_grammar(treenode *head, const vecs &code) {
    /**
    * 生成语法分析树，
    * 注：仅仅生成一行
    */

    size_t n = code.size();
    if (!n) return head;

    for (const auto& i : aslist) {
        // 赋值运算
        if (check_in(i, code)) {
            assign(head, i, code);
            return head;
        }
    }
    if (check_in(code[0], sentences)) {
        // 语句
        sentence_tree(head, code);
        return head;
    }

    // 语句块
    if (code.back() == "{")
        if (block(head, code))
            return head;

    // 返回in_func_char中的元素在code中第一次出现的位置
    int oper_i = os_check_in_s_i(in_func_char, code);
    if ((s_check_in_s(func_symbol, code)) && (oper_i == -1 || in_func(oper_i, code))) {
        // 函数
        // 考虑函数调用特殊情况，函数返回值作为运算符参数
        function(head, code);
        return head;
    }

    OPER_TREE(highest_condits, code);
    // 特殊判断， not
    if (check_in(string("not"), code)) {
        not_tree(head, code);
        return head;
    }
    OPER_TREE(other_condits, code);

    OPER_TREE(opers, code);
    // 无法匹配且为字符开头，报错
    string tmp;
    for(const auto& j : code)
        tmp += j;
    if(tmp.length() >= 1 && is_english(tmp[0]))
        send_error(NameError, tmp.c_str());
    return head;
}

static void lex_grammar(const vecs &code)
{
    /**
     * 该函数用于解析行
     * 当遇到;符号时，不断递归直到没有；为止
     */
    auto *n = new treenode;
    n = real_grammar(n, codes_s[LINE_NOW]);
    result->push_back(n);
}

void grammar(vector<treenode *> &result_return, const vecs2d &codes) {
    /**
     * 该函数是被外界所调用的，生成由语法书节点组成的vector
     */
    LINE_NOW = 0;
    size_t n = codes.size();

    auto* pointer = &LINE_NOW;
    result = &result_return;
    codes_s = codes;
    for (; *pointer < n; ++(*pointer))
        lex_grammar(codes_s[*pointer]);

    *pointer = 0;
    map_arr::clear();
}

#undef OPER_TREE
