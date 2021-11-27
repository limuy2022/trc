#include <vector>
#include <string>
#include <cstring>
#include <array>
#include "Compiler/compile_def.hpp"
#include "base/utils/data.hpp"
#include "base/Error.h"
#include "base/trcdef.h"
#include "Compiler/compile_def.hpp"
#include "token_env.h"
#include "Compiler/Compiler.h"

using namespace std;

// 应当分割的符号
static array<string, 9> symbols = {"(", ")", "[", "]", "{", "}", "#", ",", "."};

static inline void put_in(vecs &in, string &value) {
    /**
     * 填充并清空value的值
     * in：需要填充的vector
     * value：需要填充并清空的值
     */

    in.push_back(value);
    value.clear();
}

/* 将指针调整至上一个字符 */
#define set_back_char \
do{\
    --char_num;\
    --char_ptr;\
} while(0)

namespace trc {
    namespace compiler {
        bool in_all(const string& temp) {
            /**
             * 在所有标识符中查找，是否存在？
             * temp：需要查找的标识符
             */

            return (utils::check_in(temp, symbols) || \
                utils::check_in(temp, sentences) || \
                utils::check_in(temp, keywords) || \
                utils::check_in(temp, condits) || \
                utils::check_in(temp, aslist) || \
                utils::check_in(temp, opers));
        }

        string lex_string(const string& var) {
            /**
             * 解析string，并把特殊字符转义，例如特殊字符\n等
             */
            string result;
            for (size_t i = 0, n = var.length(); i < n; ++i)
                if (var[i] == '\\') {
                    if (i == n - 2) {
                        // 如果i为var最后一个值
                        error::send_error(error::SyntaxError, R"(char "\" can't be the last char for the string.)");
                    }
                    result += change_varchar[var[++i]];
                }
                else
                    result += var[i];
            return result;
        }

        vecs token_lex::get_line() {
            /**
            * 解析文件，返回token
            * codes：预处理完的代码
            * 注意：该函数保证：除非到达文件末尾，否则一定返回有意义的tokens
            */

            vecs result;
            if(*char_ptr == code.back()) {
                return result;
            }
            string temp;
            size_t t_len;

            for (int char_num = 0; !((*char_ptr == '\n' && !result.empty()) || *char_ptr == '\0'); ++char_ptr, ++char_num) {
                temp += *char_ptr;
                t_len = temp.length();
                if (*char_ptr == '#') {
                    /*忽略注释*/
                    while (*char_ptr != '\n' && *char_ptr != '\0') {
                        ++char_ptr;
                    }
                    break;
                }
                if ((temp[0] == '\'' || temp[0] == '"') && \
                    *char_ptr == temp[0] && \
                    t_len > 1) {
                    // 字符串的解析
                    temp = lex_string(temp);
                    put_in(result, temp);
                }
                else if (temp == " " || temp == "\t") {
                    temp.clear();     // 忽略空格，制表符（字符串除外)
                }

                else if (in_all(temp)) {
                    // 判断有歧义的符号
                    // 如果为负号且为第一个数前一个字符不为数字或者所有字符
                    if (temp == "-" && (char_num == 0 || in_all(temp.substr(temp.length()-2, 1))))
                        continue;
                    if (*(char_ptr+1)!='\0'&& *(char_ptr + 1) != '\n') {
                        string nextz = temp + *(char_ptr + 1);
                        if (in_all(nextz)) {
                            set_back_char;
                            put_in(result, nextz);
                            temp.clear();
                        } else
                            put_in(result, temp);
                    } else
                        put_in(result, temp);
                } else if (is_num(temp[0]) && (!is_num(*char_ptr) || (*char_ptr == '-' && t_len != 1))) {
                    // 数字
                    // 当前字符不在数字范围内,或下一个字符为符号且不为第一个字符，说明已结束
                    temp = temp.substr(0, t_len - 1);
                    put_in(result, temp);
                    set_back_char;
                } else if (is_english(temp[0]) && (!is_english_num(*char_ptr))) {
                    //当前字符不在字符范围内，说明已结束
                    temp = temp.substr(0, t_len - 1);
                    put_in(result, temp);
                    set_back_char;
                }
            }
            // 将最后一个未来得及添加的词标添加进去
            if(!temp.empty()) {
                result.push_back(temp);
            }
            (*line)++;
            return result;
        }

        token_lex::token_lex(const string &code):
            code(code),
            line(&LINE_NOW),
            char_ptr((char*)& code.front())
        {
            *line = 0;
        }

        token_lex::~token_lex() {
            *line = 0;
        }
    }
}

#undef set_back_char
