/**
 * 校验代码，检查出基本的代码错误
 * 注意：在token划分之后运行
 */

#include <stack>
#include <string>
#include <map>
#include "share.h"
#include "Error.h"
#include "Compiler/compile_share.h"
#include "data.hpp"

using namespace std;

// 括号对应
static map<char, string> key = {
        {'(',  ")"},
        {'[',  "]"},
        {'{',  "}"},
        {'\'', "'"},
        {'\"', "\""}
};

static void grammar_check(const vecs2d &codes) {
    /**
     * 检查语法是否有问题
     * 检验符号是否匹配问题，包括{，[,(等符号
     */

    stack<char> t;
    for (const auto &i: codes) {
        // 防止空行出现
        if (i.empty()) continue;
        // 语句块标志出现，{必须在它后面
        if (check_in(i.front(), keywords) && i.back() != "{")
            send_error(SyntaxError, (string("{ must behind the keyword ") + i.front() + " on the same line.").c_str());

        // 使用栈验证整个程序是否存在括号匹配问题
        for (const auto &j: i) {
            if (j == "[" || j == "{" || j == "(" || j == "'" || j == "\"") {
                t.push(j[0]);
            } else if (j == ")" || j == "]" || j == "}" || j == "'" || j == "\"") {
                if (t.empty() || key[t.top()] != j)
                    send_error(SyntaxError, (string("there is too many ") + j).c_str());
                t.pop();
            }
        }
        LINE_NOW++;
    }
    if (!t.empty())
        send_error(SyntaxError, "SyntaxError");
}

void check(const vecs2d &codes) {
    /**
     * codes：词法分析完的代码
     * 检查程序是否存在语法漏洞（并不是所有都在此处检查）
     */

    LINE_NOW = 0;
    // 进行语法检查
    grammar_check(codes);
}
