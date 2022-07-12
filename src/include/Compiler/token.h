#pragma once

#include <Compiler/Compiler.h>
#include <Compiler/pri_compiler.hpp>
#include <stack>

namespace trc::compiler {
/**
 * @brief token
 * @details 一个完整的token包括标识和值两部分,是解析器的基本单元
 */
class TRC_Compiler_api token {
public:
    // 标识
    token_ticks tick;
    // 值
    char* data;

    token(token_ticks, const char* data, size_t len);

    token(token_ticks);

    token() = default;

    /**
     * @brief 设置字符串大小
     * @param len 字符串长度(不包括\0)
     */
    void set_size(size_t len);

    ~token();
};

/**
 * @brief
 * 这是一个将字符串转换成token流按行输出的类
 */
class TRC_Compiler_api token_lex {
public:
    token_lex(const std::string& code, compiler_public_data& compiler_data);

    ~token_lex();

    /**
     * @brief
     * 从当前字符串代码中读取一个token并返回给grammar解析
     *
     * @return token* 返回一个有意义的token
     * @return
     * 特殊返回值：由于总是保证返回有意义，所以当token的tick为token_ticks::END_OF_TOKENS时，表示解析结束
     */
    token* get_token();

    void unget_token(token* token_data);

    compiler_public_data& compiler_data;

private:
    // 标记是否有token储存着
    bool is_unget = false;

    token* back_token = nullptr;

    // 经过简单处理的原始字符串代码
    const std::string& rawcode;

    // 指向当前正在解析的字符
    const char* char_ptr;

    // 判断是否解析到了终点
    bool end_of_lex() const noexcept;

    // 解析数字（包括浮点数）
    token* lex_int_float();

    /**
     * @brief 解析一个字符串
     * 注：会略过开头结尾的"和'符号
     */
    token* lex_string();

    /**
     * @brief
     * 解析英文符号（包含关键字和名称两种可能）
     */
    token* lex_english();

    /**
     * @brief 解析其他字符，如[],()等
     * @details
     * 在这里解析的字符都能被用token_ticks完整表达，所以不需要储存任何信息
     */
    token* lex_others();

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

    // 用于检查代码是否正确
    std::stack<char> check_brace;
};
}