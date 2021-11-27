/**
 * 编译时期共享的数据
 */

#include <string>
#include <map>
#include <array>
#include "Compiler/compile_def.hpp"
#include "base/Error.h"

using namespace std;

namespace trc
{
    namespace compiler
    {
        treenode::treenode(grammar_type type_argv, const string &data) : data((char *)malloc(sizeof(char) + data.length() + 1)),
                                                                         type(type_argv),
                                                                         is_alloc(true) {}

        treenode::treenode(grammar_type type) : type(type) {}

        void treenode::connect(treenode *son_connect)
        {
            /**
             * 本函数负责将父节点和子节点联系在一起
             */
            son_connect->father = this;
            son.push_back(son_connect);
        }

        treenode::~treenode()
        {
            if (is_alloc)
                free(data);
        }

        void treenode::set_alloc(size_t sizes)
        {
            /**
             * 重新设置大小，包括\0
             */
            if (!is_alloc)
            {
                is_alloc = true;
                data = (char *)(malloc(sizeof(char) * sizes + 1));
            }
            else
            {
                data = (char *)realloc(data, sizeof(char) * sizes + 1);
            }
        }

        COMPILE_TYPE_TICK what_type(const string &value)
        {
            /**
             * 编译时判断传入的字符串实际代表的类型
             * return:一个字符串，类型
             */

            char front = value.front(), back = value.back();
            size_t lenght = value.length();
            if (('\'' == front && '\'' == back) || (front == '"' && back == '"'))
                return string_TICK;

            if (utils::check_in(front, num))
            {
                if (value.find('.') != string::npos)
                {
                    // 小数
                    if (lenght >= 12)
                        return compiler::FLOAT_L_TICK;
                    return compiler::float_TICK;
                }
                // 检查数据,前导零
                if (lenght >= 2 && front == '0')
                    error::send_error(error::SyntaxError, ("number " + value + " is incorrect.").c_str());
                if (lenght >= 12)
                    return LONG_TICK;

                return int_TICK;
            }
            if (utils::check_in(value, const_values))
                return CONST_TICK;
            return VAR_TICK;
        }

        array<char, 12> num = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', '-'};
        // 语句
        array<string, 4> sentences = {"import", "goto", "del", "assert"};
        // 带有参数的语句（满足有且只有一个的语句）
        array<string, 1> sentences_yes_argv = {"goto"};
        // 关键字
        array<string, 5> keywords = {"if", "while", "for", "func", "class"};
        // 最高级运算符
        array<string, 2> highest_condits = {"and", "or"};
        // 除了最高级运算符以外的运算符
        array<string, 6> other_condits = {"<", ">", "<=", ">=", "!=", "=="};
        // 所有运算符
        array<string, 9> condits = {"<", ">", "<=", ">=", "!=", "==", "and", "or", "not"};
        // 等号
        array<string, 2> aslist = {":=", "="};
        // 运算符
        array<string, 14> opers = {
            "+",
            "-",
            "**",
            "*",
            "/",
            "//",
            "%",
            "+=",
            "-=",
            "*=",
            "/=",
            "**=",
            "%=",
            "//=",
        };
        // 可以优化的运算符
        array<string, 7> opti_opers = {
            "+", "-", "**",
            "*", "//", "%", "/"};
        // 可以优化的条件运算符
        array<string, 8> opti_condits = {
            "<", ">", "<=", ">=",
            "!=", "==", "and", "or"};
        // 函数有时与其它运算符优先级关系并不确定，需要通过括号判定，in_func_char包括了这些字符
        array<string, 15> in_func_char = {
            "+", "-", "**", "*", "/", "//",
            "%", "<", ">", "<=", ">=", "!=",
            "==", "and", "or"};
        // 转义字符对应表
        map<char, string> change_varchar = {{'n', "\n"},
                                            {'t', "\t"},
                                            {'\'', "'"},
                                            {'"', R"(")"},
                                            {'\\', R"(\)"}};
        // 常量值
        array<string, 3> const_values = {
            "true",
            "false",
            "null"};
        // 常量值到数字的转换
        map<string, int> change_const = {
            {"true", 1},
            {"false", 0},
            {"null", 0},
        };
    }
}
