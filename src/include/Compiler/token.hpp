#pragma once

#include <Compiler/compiler_def.hpp>
#include <stack>

namespace trc::compiler {
// token的标识
enum class token_ticks : size_t {
    FOR, // for
    WHILE, // while
    IF, // if
    FUNC, // function
    CLASS, // class
    ADD, // +
    SUB, // -
    MUL, // *
    DIV, // /
    ZDIV, // //
    MOD, // %
    POW, // **
    AND, // and
    OR, // or
    NOT, // not
    EQUAL, // ==
    UNEQUAL, // !=
    LESS, // <
    GREATER, // >
    LESS_EQUAL, // <=
    GREATER_EQUAL, // >=
    IMPORT, // import
    GOTO, // goto
    DEL, // del
    ASSERT, // assert
    BREAK, // break
    CONTINUE, // continue
    SELFADD, // +=
    SELFSUB, // -=
    SELFMUL, // *=
    SELFDIV, // /=
    SELFZDIV, // //=
    SELFMOD, // %=
    SELFPOW, // **=
    ASSIGN, // =
    STORE, // :=
    NAME, // 名称
    NULL_, // null
    TRUE_, // true
    FALSE_, // false
    STRING_VALUE, // 字符串值
    LONG_FLOAT_VALUE, // 长浮点型值
    FLOAT_VALUE, // 浮点数值
    LONG_INT_VALUE, // 长整型值
    INT_VALUE, // 整型值
    LEFT_BIG_BRACE, // {
    RIGHT_BIG_BRACE, // }
    LEFT_SMALL_BRACE, // (
    RIGHT_SMALL_BRACE, // )
    LEFT_MID_BRACE, // [
    RIGHT_MID_BRACE, // ]
    POINT, //.
    COMMA, // ,
    UNKNOWN, // unknown value,可以暂时用来占个位
    END_OF_TOKENS, // 解析结束
    END_OF_LINE, // 行结束
};

/**
 * @brief token
 * @details 一个完整的token包括标识和值两部分,是解析器的基本单元
 */
struct token {
    // 标识
    token_ticks tick {};
    // 值
    size_t data = 0;
};

/**
 * @brief
 * 这是一个将字符串转换成token流按行输出的类
 */
class token_lex {
public:
    explicit token_lex(compiler_public_data& compiler_data);

    void set_code(const std::string& code);

    ~token_lex();

    /**
     * @brief
     * 从当前字符串代码中读取一个token并返回给grammar解析
     *
     * @return token 返回一个有意义的token
     * @return
     * 特殊返回值：由于总是保证返回有意义，所以当token的tick为token_ticks::END_OF_TOKENS时，表示解析结束
     */
    token get_token();

    /**
     * @brief 退回并储存一个token
     */
    void unget_token(token token_data);

    compiler_public_data& compiler_data;

private:
    token back_token;
    bool has_back_token = false;

    // 指向当前正在解析的字符
    const char* char_ptr {};

    // 判断是否解析到了终点
    [[nodiscard]] bool end_of_lex() const noexcept;

    // 解析数字（包括浮点数）
    token lex_int_float();

    /**
     * @brief 解析一个字符串
     * 注：会略过开头结尾的"和'符号
     */
    token lex_string();

    /**
     * @brief
     * 解析英文符号（包含关键字和名称两种可能）
     */
    token lex_english();

    /**
     * @brief 解析其他字符，如[],()等
     * @details
     * 在这里解析的字符都能被用token_ticks完整表达，所以不需要储存任何信息
     */
    token lex_others();

    /**
     * 解析符号时遇到多种情况，例如读取*后可以为*，*=，**，**=四种情况
     * 使用方法：传入符号后期待的符号，如*期待=,再依次传入期待满足时的标记和期待不满足时的标记
     */
    token_ticks get_binary_ticks(
        char expected_char, token_ticks expected, token_ticks unexpected);

    /**
     * 只有在有且仅有一个期待字符时使用
     * @brief
     * 检查下一个字符是不是期待的字符，如果接下来是不是期待的字符，直接报错
     *
     * @param expected_char 接下来唯一期待的字符
     */
    void check_expected_char(char expected_char);

    // 用于检查括号是否正确匹配
    std::stack<char> check_brace;
};
}
