#include <vector>
#include <string>
#include "../include/Compiler/compile_share.h"
#include "../include/data.hpp"
#include "../include/Error.h"
#include "../include/cfg.h"

using namespace std;

// 应当分割的符号
const vecs symbols = {"(", ")", "[", "]", "{", "}", "#", ",", "."};

static void put_in(vecs &in, string &value) {
    /**
     * 填充并清空value的值
     * in：需要填充的vector
     * value：需要填充并清空的值
     */

    in.push_back(value);
    value = "";
}

static bool in_all(const string &temp) {
    /**
     * 在所有标识符中查找，是否存在？
     * temp：需要查找的标识符
     */

    return (check_in(temp, symbols) || \
            check_in(temp, sentences) || \
            check_in(temp, keywords) || \
            check_in(temp, condits) || \
            check_in(temp, aslist) || \
            check_in(temp, opers));
}

static string lex_string(const string &var) {
    /**
     * 解析string，并把特殊字符转义，例如特殊字符\n等
     */
    string result;
    size_t n = var.length();
    for (int i = 0; i < n; ++i)
        if (var[i] == '\\') {
            if (i == n - 2) {
                // 如果i为var最后一个值
                send_error(SyntaxError, R"(char "\" can't be the last char for the string.)");
            }
            result += change_varchar[var[++i]];
        } else
            result += var[i];
    return result;
}

static bool last_char_uneq(const string &code, int index) {
    /**
     * 上一个字符是否在所有标识符中，注：略过空格
     * code：当前行
     * index：当前字符在当前行中的索引
     */
    while (code[index] == ' ') index--;
    string temp;
    temp += code[index];
    return in_all(temp);
}

void token(vecs2d &result, const vecs &codes) {
    /**
    * 解析文件，返回token
    * codes：预处理完的代码
    */

    string temp;
    vecs add_temp;
    size_t t_len;

    LINE_NOW = 0;
    auto *pointer = &LINE_NOW;

    for (const auto &line : codes) {
        size_t n = line.length();
        for (int j = 0; j < n; ++j) {
            temp += line[j];
            t_len = temp.length();
            if ((temp[0] == '\'' || temp[0] == '"') && \
                temp[t_len - 1] == temp[0] && \
                t_len > 1) {
                // 字符串的解析
                temp = lex_string(temp);
                put_in(add_temp, temp);
            } else if (temp == " ")
                temp.clear();     // 忽略空格（字符串除外)
            else if (in_all(temp)) {
                // 判断有歧义的符号
                // 如果为负号且为第一个数前一个字符不为数字或者所有字符
                if (temp == "-" && (j == 0 || last_char_uneq(line, j - 1))) continue;
                if (j + 1 < n) {
                    string nextz = temp + line[j + 1];
                    if (in_all(nextz)) {
                        j++;
                        put_in(add_temp, nextz);
                        temp.clear();
                    } else
                        put_in(add_temp, temp);
                } else
                    put_in(add_temp, temp);
            } else if (is_num(temp[0]) && (!is_num(line[j]) || (line[j] == '-' && t_len != 1))) {
                // 数字
                // 当前字符不在数字范围内,或下一个字符为符号且不为第一个字符，说明已结束
                temp = temp.substr(0, t_len - 1);
                put_in(add_temp, temp);
                j--;
            } else if (is_english(temp[0]) && (!is_english_num(line[j]))) {
                //当前字符不在字符范围内，说明已结束
                temp = temp.substr(0, t_len - 1);
                put_in(add_temp, temp);
                j--;
            }
        }
        // 将最后一个未来得及添加的词标添加进去
        add_temp.push_back(temp);
        temp.clear();

        // 优化，避免""元素进入其中
        vecs result_temp;
        const auto& end_ = add_temp.end();
        for (auto i = add_temp.begin(); i < end_; ++i) {
            if (!(i->empty())) {
                result_temp.push_back(*i);
            }
        }
        result.push_back(result_temp);
        add_temp.clear();
        result_temp.clear();
        (*pointer)++;
    }
    *pointer = 0;
}

#undef is_num
#undef is_english_num
#undef is_english
