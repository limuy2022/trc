/**
 * 工具：输出token，调试工具
 */

#include <Compiler/Compiler.h>
#include <Compiler/token.h>
#include <base/utils/filesys.h>
#include <cstdio>
#include <tools.h>

// token标记映射到名称，便于输出
// todo:从最新的token进行更新
static const char* tokenticks_to_string[] = {
    "FOR", // for
    "WHILE", // while
    "IF", // if
    "FUNC", // function
    "CLASS", // class
    "ADD", // +
    "SUB", // -
    "MUL", // *
    "DIV", // /
    "ZDIV", // //
    "MOD", // %
    "POW", // **
    "AND", // and
    "OR", // or
    "NOT", // not
    "EQUAL", // ==
    "UNEQUAL", // !=
    "LESS", // <
    "GREATER", // >
    "LESS_EQUAL", // <=
    "GREATER_EQUAL", // >=
    "IMPORT", // import
    "GOTO", // goto
    "DEL", // del
    "BREAK", // break
    "CONTINUE", // continue
    "ASSERT", // assert
    "SELFADD", // +=
    "SELFSUB", // -=
    "SELFMUL", // *=
    "SELFDIV", // /=
    "SELFZDIV", // //=
    "SELFMOD", // %=
    "SELFPOW", // **=
    "ASSIGN", // =
    "STORE", // :=
    "NAME", // 名称
    "STRING_VALUE", // 字符串值
    "FLOAT_VALUE", // 浮点数值
    "INT_VALUE", // 整型值
    "LEFT_BIG_BRACE", // {
    "RIGHT_BIG_BRACE", // }
    "LEFT_SMALL_BRACE", // (
    "RIGHT_SMALL_BRACE", // )
    "LEFT_MID_BRACE", // [
    "RIGHT_MID_BRACE", // ]
    "POINT", //.
    "COMMA", // ,
};

namespace trc::tools {
namespace tools_in {
    /**
     * @brief 格式化输出
     */
    static void out(
        const std::string& file_name, compiler::token_lex& token_c) {
        printf("From file %s:", file_name.c_str());
        compiler::token* token_lex;
        bool change_line = true;
        for (;;) {
            if (change_line) {
                printf("\n%zu:", token_c.compiler_data.error.line);
                change_line = false;
            }
            token_lex = token_c.get_token();
            if (token_lex->tick == compiler::token_ticks::END_OF_TOKENS) {
                return;
            }
            if (token_lex->tick == compiler::token_ticks::END_OF_LINE) {
                change_line = true;
                continue;
            }
            printf("%s, %s", token_lex->data,
                tokenticks_to_string[(int)token_lex->tick]);
        }
    }

    void _out_token(const std::string& path) {
        std::string file_data;
        utils::readcode(file_data, path);
        compiler::compiler_public_data compiler_data { path,
            compiler::nooptimize_option };
        // 解析
        compiler::token_lex token_c(file_data, compiler_data);
        out(path, token_c);
    }
}

namespace tools_out {
    void out_token() {
        for (int i = 2; i < argc; ++i) {
            tools_in::_out_token(argv[i]);
        }
    }
}
}
