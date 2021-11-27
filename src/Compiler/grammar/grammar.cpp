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
#include <cmath>
#include <array>
#include <stack>
#include "base/trcdef.h"
#include "base/utils/data.hpp"
#include "Compiler/optimize.h"
#include "base/utils/type.hpp"
#include "base/func_loader.h"
#include "base/Error.h"
#include "grammar_env.h"
#include "Compiler/Compiler.h"
#include "base/utils/types.h"

using namespace std;

// 运算符和条件解析
#define OPER_TREE(list_var_, __code_)      \
    for (auto &i : (list_var_))            \
    {                                      \
        if (utils::check_in(i, (__code_))) \
        {                                  \
            oper_tree(head, (__code_), i); \
            return head;                   \
        }                                  \
    }

// 函数标志
static array<string, 2> func_symbol = {"(", ")"};

static vecs cut(const vecs &origin, const string &start, const string &end)
{
    /**
     *功能性函数，切取start到end的距离，具体规则参考python的切片
     *“切头不切尾”
     */

    const auto &start_iter = origin.begin(), &end_iter = origin.end();
    return vecs(find(start_iter, end_iter, start), find(start_iter, end_iter, end));
}

static vecs until_cut(const vecs &code, const string &cut_sym)
{
    /**
     * 切除直到遇到某个字符串
     */

    const auto &start = code.begin();
    return vecs(start, find(start, code.end(), cut_sym));
}

static vecs from_cut_until(const vecs &code, const string &cut_sym)
{
    /**
     * 从某个字符串开始切到末尾
     */

    const auto &end = code.end(), &start = find(code.begin(), end, cut_sym) + 1;
    return vecs(start, end);
}

static bool in_func(int index_oper, const vecs &code)
{
    /**
     * 判断某个符号是否处于函数括号中
     * @param index_oper : 符号所在的索引
     * @param code : 符号所在的语句
     */

    const auto &start_iter = code.rbegin(), &end_iter = code.rend();
    int start_sym = trc::utils::index_vector(code, "("),
        end_sym = abs(distance(end_iter, (find(start_iter, end_iter, ")")))) - 1;
    return (start_sym < index_oper && end_sym > index_oper);
}

namespace trc
{
    namespace compiler
    {
        class detail_grammar
        {
            /**
             * 语言的解析细节
             */
        public:
            treenode *get_node(const vecs &);

            detail_grammar(vector<treenode *> *line);

        private:
            inline void save_line(trc::compiler::blocks_type type)
            {
                /**
                 * 保存行及信息
                 * 例如while将保存while和while所在的行数
                 */
                blocks_st.push(type);
                lines_save.push(*line_);
            }

            treenode *ifis(const vecs &code);

            void assign(treenode *head, const string &oper, const vecs &code);

            void callfunction(treenode *head, const vecs &code);

            void sentence_tree(treenode *head, const vecs &code);

            void while_loop_tree(treenode *head, const vecs &code);

            void if_tree(treenode *head, const vecs &code);

            void func_define(treenode *head, const vecs &code);

            bool block(treenode *head, const vecs &code);

            void oper_tree(treenode *head, const vecs &code, const string &oper);

            void not_tree(treenode *head, const vecs &code);

            /**
             * 记录相关的整型信息
             * 例如：while a>0{
             *      print(a)
             *      a=a-1
             * }
             * 将会把while a>0{所在的行数存进去，直到遇到}再取出
             */
            stack<int> lines_save;
            /*储存代码块的信息，例如遇到if将if的信息存入栈中，遇到}再取出*/
            stack<trc::compiler::blocks_type> blocks_st;

            int *line_;

            vector<treenode *> *codes;
        };
        treenode *detail_grammar::ifis(const vecs &code)
        {
            /**
             * 本节点是对real_grammar函数的间接递归
             * 当code为数据时返回DATA节点
             * 当code为表达式时解析代码并返回TREE节点
             * 注：本函数仅仅生成节点，对于指向不作管理
             */

            return code.size() == 1 ? new treenode(DATA, code.front()) : get_node(code);
        }

        void detail_grammar::assign(treenode *head, const string &oper, const vecs &code)
        {
            /**
             * 生成赋值语句节点
             */

            // 申请新的树节点
            auto ass = new treenode,
                 name = new treenode,
                 opernode = new treenode(VAR_DEFINE, oper);
            // 父节点指向
            const vecs &temp = until_cut(code, oper);
            if (temp.size() == 1)
            {
                // 数组之类的名字比一个长
                name->set_alloc(temp[0].length());
                strcpy(name->data, temp[0].c_str());
                name->type = DATA;
            }
            else
            {
            }

            // 父节点留下子节点记录
            head->connect(ass);
            opernode->connect(name);
            // 存放等号右边的值
            ass->connect(ifis(from_cut_until(code, oper)));
            ass->connect(opernode);
        }

        void detail_grammar::callfunction(treenode *head, const vecs &code)
        {
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
            if (argv_n != 1)
            {
                for (const auto &i : argv)
                {
                    if (i == "(")
                        break_num++;
                    if ((i == "," || i == ")") && break_num == 0)
                    {
                        argv_node->connect(ifis(add_temp));
                        argc++;
                        add_temp.clear();
                    }
                    else
                        add_temp.push_back(i);
                    if (i == ")")
                        break_num--;
                }
            }

            // 由于栈先进后出的特征，在此处将参数进行反转
            vector<treenode *> finally_temp;
            size_t f_n = argv_node->son.size();
            if (f_n > 1)
            {
                for (int i = 1; i <= f_n; ++i)
                {
                    finally_temp.push_back(argv_node->son[f_n - i]);
                }
                argv_node->son = finally_temp;
            }

            add_temp.clear();
            auto *len = new treenode(DATA, to_string(argc));
            functree->connect(len);

            // 函数名问题：判断内置函数和自定义函数
            if (name.size() == 1)
            {
                if (utils::check_in(name[0], loader::num_func))
                {
                    // 内置函数
                    auto *builtin = new treenode(BUILTIN_FUNC), *bycode = new treenode(DATA), *nodeargv = new treenode(DATA);
                    bycode->data = "CALL_BUILTIN";
                    int tmp = loader::func_num[name[0]];
                    nodeargv->set_alloc(utils::len(tmp) + 1);
                    itoa(tmp, nodeargv->data, 10);
                    builtin->connect(bycode);
                    builtin->connect(nodeargv);
                    functree->connect(builtin);
                }
                else
                {
                    auto *builtin = new treenode(CALL_FUNC), *bycode = new treenode(DATA), *nodeargv = new treenode(DATA);
                    bycode->data = "CALL_FUNCTION";
                    int tmp = loader::func_num[name[0]];
                    nodeargv->set_alloc(utils::len(tmp) + 1);
                    itoa(tmp, nodeargv->data, 10);
                    builtin->connect(bycode);
                    builtin->connect(nodeargv);
                    functree->connect(builtin);
                }
            }
        }

        void detail_grammar::sentence_tree(treenode *head, const vecs &code)
        {
            /**
             * 生成语句执行节点
             */

            auto *argv_node = new treenode;
            const string &sentence_name = code[0];
            size_t n = code.size(), argc = 0;
            if (n == 1)
            {
                // 单个语句
                auto *snode = new treenode(ORIGIN, sentence_name);
                head->connect(snode);
                return;
            }
            else if (utils::check_in(code[0], sentences_yes_argv))
            {
                auto *snode = new treenode(OPCODE_ARGV, code[0]),
                     *argv = new treenode(DATA, code[1]);
                snode->connect(argv);
                head->connect(snode);
                return;
            }
            auto *snode = new treenode;

            const vecs &argv = from_cut_until(code, code[0]);
            vecs add_temp;
            size_t argv_n = argv.size();

            for (int i = 0; i < argv_n; ++i)
            {
                if (argv[i] == ",")
                {
                    argv_node->connect(ifis(add_temp));
                    ++argc;
                    add_temp.clear();
                }
                else
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

        void detail_grammar::while_loop_tree(treenode *head, const vecs &code)
        {
            /**
             * 生成条件循环字节码
             * 注：仅对当前行进行解析
             * 循环通过跳转实现
             */

            const auto &begin_ = code.begin(),
                       &end_ = code.end();
            // 条件表达式
            const vecs &bool_sen = cut(code, *(find(begin_, end_, "while") + 1), "{");
            // 保存当前循环状态
            save_line(blocks_type::WHILE_TYPE);
            auto *opcode_ = new treenode(OPCODE_ARGV, "if"),
                 *data_node = new treenode(DATA, ""); //先使用空字符占位，当遇到}时再补回来
            opcode_->connect(data_node);
            head->connect(ifis(bool_sen));
            head->connect(opcode_);
        }

        void detail_grammar::if_tree(treenode *head, const vecs &code)
        {
            /**
             * 生成条件判断字节码
             * 注：仅对当前行进行解析
             * 判断通过跳转实现
             */

            const auto &begin_ = code.begin(),
                       &end_ = code.end();
            // 条件表达式
            const vecs &bool_sen = cut(code, *(find(begin_, end_, "if") + 1), "{");
            save_line(blocks_type::IF_TYPE);
            auto *opcode_ = new treenode(OPCODE_ARGV, "if"),
                 *data_node = new treenode(DATA, "");
            opcode_->connect(data_node);
            head->connect(ifis(bool_sen));
            head->connect(opcode_);
        }

        void detail_grammar::func_define(treenode *head, const vecs &code)
        {
            /**
             *定义函数，原理：标记出函数结束的位置，使字节码生成器把函数单独解析
             * 使用goto语句改变定义
             */

            save_line(blocks_type::FUNC_TYPE);
            auto *func_node = new treenode(FUNC_DEFINE, code[1]),
                 *line_node = new treenode(DATA, "");

            func_node->connect(line_node);
            head->connect(func_node);
        }

        bool detail_grammar::block(treenode *head, const vecs &code)
        {
            /**
             * 此函数负责所有关于语句块的语句的解析
             * 包括func，if， while等
             */

            const char *name = code[0].c_str();
            if (!strcmp(name, "if"))
            {
                if_tree(head, code);
                return true;
            }
            if (!strcmp(name, "while"))
            {
                while_loop_tree(head, code);
                return true;
            }
            if (!strcmp(name, "func"))
            {
                func_define(head, code);
                return true;
            }
            return false;
        }

        void detail_grammar::oper_tree(treenode *head, const vecs &code, const string &oper)
        {
            /**
             * 运算符和条件运算符解析，+-*%,==,!=等运算符
             */

            auto oper_node = new treenode(TREE),
                 oper_data = new treenode(OPCODE, oper),
                 left = ifis(until_cut(code, oper)),
                 right = ifis(from_cut_until(code, oper));

            if (left->type == DATA &&
                right->type == DATA &&
                what_type(left->data) == int_TICK &&
                what_type(right->data) == int_TICK)
            {
                auto *result_oper = new treenode(DATA);
                // 仅允许整型进行运算符的常量折叠
                if (utils::check_in(oper, opti_condits))
                {
                    // 布尔值
                    int res_tmp = optimize_condit[oper](atoi(left->data), atoi(right->data));
                    result_oper->set_alloc(utils::len(res_tmp) + 1);
                    itoa(res_tmp, result_oper->data, 10);
                    head->connect(result_oper);
                    return;
                }
                else if (utils::check_in(oper, opti_opers))
                {
                    bool res_tmp = optimize_number[oper](atoi(left->data), atoi(right->data));
                    result_oper->set_alloc(2);
                    result_oper->data[1] = '\0';
                    result_oper->data[0] = res_tmp ? '1' : '0';
                    head->connect(result_oper);
                    return;
                }
            }
            oper_node->connect(left);
            oper_node->connect(right);
            oper_node->connect(oper_data);
            head->connect(oper_node);
        }

        void detail_grammar::not_tree(treenode *head, const vecs &code)
        {
            /**
             * !非运算符
             */

            head->connect(ifis(from_cut_until(code, "not")));
            head->connect(new treenode(OPCODE, "not"));
        }

        treenode *detail_grammar::get_node(const vecs &code)
        {
            auto head = new treenode;
            size_t n = code.size();
            if (!n)
                return head;

            for (auto &i : aslist)
            {
                // 赋值运算
                if (utils::check_in(i, code))
                {
                    assign(head, i, code);
                    return head;
                }
            }
            if (utils::check_in(code[0], sentences))
            {
                // 语句
                sentence_tree(head, code);
                return head;
            }

            // 语句块
            const string &end = code.back();
            if (end == "{")
                if (block(head, code))
                    return head;
            if (end == "}")
            {
                // 语句块结束，需要修正代码
                int start_line = lines_save.top();
                switch (blocks_st.top())
                {
                case blocks_type::WHILE_TYPE:
                {
                    head = ifis({"goto", to_string(start_line)});
                    // 覆盖当时的占位符
                    head->son[1]->son[0]->set_alloc(utils::len(*line_));
                    itoa(*line_, head->son[1]->son[0]->data, 10);
                    break;
                }
                case blocks_type::IF_TYPE:
                {
                    // 覆盖当时的占位符
                    head->son[1]->son[0]->set_alloc(utils::len(*line_));
                    itoa(*line_, head->son[1]->son[0]->data, 10);
                    break;
                }
                }
                blocks_st.pop();
                lines_save.pop();
            }

            // 返回in_func_char中的元素在code中第一次出现的位置
            int oper_i = utils::os_check_in_s_i(in_func_char, code);
            // 判断语句含义：如果（）符号都在语句中且不为add(1, 1)+r(1, 2)的形式，作为函数进行解析
            if (utils::s_check_in_s(func_symbol, code) && (oper_i == -1 || in_func(oper_i, code)))
            {
                // 函数
                callfunction(head, code);
                return head;
            }
            // 判断最高级运算符
            OPER_TREE(highest_condits, code)
            // 特殊判断， not
            if (utils::check_in("not", code))
            {
                not_tree(head, code);
                return head;
            }
            OPER_TREE(other_condits, code)

            // 运算符，优先级低
            OPER_TREE(opers, code)
            // 无法匹配且为字符开头，报错
            string tmp;
            for (const auto &j : code)
                tmp += j;
            if (tmp.length() > 0 && is_english(tmp.front()))
                error::send_error(error::NameError, tmp.c_str());
            return head;
        }

        detail_grammar::detail_grammar(vector<treenode *> *line) : line_(&LINE_NOW),
                                                                   codes(line)
        {
        }
    }
}

namespace trc
{
    namespace compiler
    {
        grammar_lex::grammar_lex(const string &codes_str) : token_(codes_str),
                                                            env(new grammar_data_control),
                                                            lex(new detail_grammar(&codes))
        {
        }

        grammar_lex::~grammar_lex()
        {
            delete env;
            delete lex;
        }

        vector<treenode *> *grammar_lex::compile_nodes()
        {
            for (;;)
            {
                const vecs &codes_to = token_.get_line();
                if (codes_to.empty())
                {
                    // 代表解析结束
                    return &codes;
                }
                codes.push_back(lex->get_node(codes_to));
            }
        }
    }
}
