/**
 * 校验代码，检查出基本的代码错误
 * 注意：在token划分之后运行
 */

#include <stack>
#include <string>
#include <map>
#include "base/Error.h"
#include "Compiler/compile_def.hpp"
#include "base/utils/data.hpp"

using namespace std;

//// 括号对应
//static map<char, string> key = {
//        {'(',  ")"},
//        {'[',  "]"},
//        {'{',  "}"},
//        {'\'', "'"},
//        {'\"', "\""}
//};
//
//static void grammar_check(const vecs &codes) {
//    /**
//     * 检查语法是否有问题
//     * 检验符号是否匹配问题，包括{，[,(等符号
//     */
//
//    stack<char> t;
//    for (const auto &i: codes) {
//        // 使用栈验证整个程序是否存在括号匹配问题
//        if (i == "[" || i == "{" || i == "(" || i == "'" || i == "\"") {
//            t.push(j[0]);
//        } else if (i == ")" || i == "]" || i == "}" || i == "'" || i == "\"") {
//            if (t.empty() || key[t.top()] != i)
//                send_error(SyntaxError, ("there is too many " + i).c_str());
//            t.pop();
//        }
//    }
//    if (!t.empty())
//        send_error(SyntaxError, "SyntaxError");
//}
//
//void check(const vecs &codes) {
//    /**
//     * codes：词法分析完的代码
//     * 检查程序是否存在语法漏洞（并不是所有都在此处检查）
//     */
//
//    // 进行语法检查
//    grammar_check(codes);
//}
//