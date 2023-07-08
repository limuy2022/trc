module;
#include <cstring>
#include <language/error.hpp>
#include <stack>
#include <string>
#include <unreach.hpp>
#include <vector>
export module token;
import compiler_def;
import trc_flong;
import trc_long;
import Error;

export namespace trc::compiler {
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

token token_lex::lex_string() {
    // 略过当前"符号
    char string_begin = *char_ptr;
    ++char_ptr;
    const char* start = char_ptr;
    while (*char_ptr != string_begin) {
        // 读入下一个符号
        if (*char_ptr == '\\') {
            char_ptr++;
        }
        if (end_of_lex()) {
            // 读到文件末尾了，说明字符串解析错误
            compiler_data.error.send_error_module(
                error::SyntaxError, language::error::syntaxerror_lexstring);
        }
        ++char_ptr;
    }
    ptrdiff_t str_len = char_ptr - start;
    std::string str;
    str.reserve(str_len);
    // 使用自定义的函数方便处理换行符，消除字符的移动
    for (char* i = const_cast<char*>(start); i != char_ptr; ++i) {
        if (*i == '\\') {
            // 转义符
            ++i;
            // 读出真实符号并匹配转为真实符号
            switch (*i) {
            case 'r': {
                str += '\r';
                break;
            }
            case 'b': {
                str += '\b';
                break;
            }
            case 'n': {
                str += '\n';
                break;
            }
            case '\'': {
                str += '\'';
                break;
            }
            case '"': {
                str += '"';
                break;
            }
            case 't': {
                str += '\t';
                break;
            }
            case '\\': {
                str += '\\';
                break;
            }
            case '0': {
                str += '\0';
                break;
            }
            case 'a': {
                str += '\a';
                break;
            }
            case 'f': {
                str += '\f';
                break;
            }
            case 'v': {
                str += '\v';
                break;
            }
            default: {
                compiler_data.error.send_error_module(error::SyntaxError,
                    language::error::syntaxerror_escape_char);
            }
            }
        } else {
            str += *i;
        }
    }
    // 略过字符串终结符
    ++char_ptr;
    return token { token_ticks::STRING_VALUE,
        compiler_data.const_string.add(str) };
}

token token_lex::lex_int_float() {
    const char* start = char_ptr;
    token_ticks tick_for_res = token_ticks::INT_VALUE;
    do {
        if (!isdigit(*char_ptr)) {
            if (*char_ptr == '.') {
                // 小数点，开启调整类型为浮点数
                tick_for_res = token_ticks::FLOAT_VALUE;
            } else if (*char_ptr != '_') {
                break;
            }
        }
        ++char_ptr;
    } while (!end_of_lex());
    token result;
    ptrdiff_t res_len = char_ptr - start;
    std::string str;
    str.reserve(res_len);
    for (char* i = const_cast<char*>(start); i != char_ptr; ++i) {
        if (*i != '_') {
            // 忽略数字中间的下划线，例如123_456
            str += *i;
        }
    }
    // 尝试纠正为长整型和长浮点型
    switch (tick_for_res) {
    case token_ticks::FLOAT_VALUE: {
        if (res_len > FLOAT_LONGFLOAT_LINE) {
            // todo
            tick_for_res = token_ticks::LONG_FLOAT_VALUE;
        } else {
            result.data = compiler_data.const_float.add(atof(str.c_str()));
        }
        break;
    }
    case token_ticks::INT_VALUE: {
        if (res_len > INT_LONGINT_LINE) {
            result.data = compiler_data.const_long_int.add(str);
            tick_for_res = token_ticks::LONG_INT_VALUE;
        } else {
            result.data = compiler_data.const_int.add(stoi(str));
        }
        break;
    }
    default: {
        UNREACH("Another token tick %zu", tick_for_res);
    }
    }
    result.tick = tick_for_res;
    return result;
}

bool token_lex::end_of_lex() const noexcept {
    return *char_ptr == '\n' || *char_ptr == '\0';
}

#define CREATE_KEYWORD(str, tick)                                              \
    { str, tick, sizeof(str) - 1 }

struct {
    const char* str;
    token_ticks tick;
    size_t len;
} keywords_[] = { CREATE_KEYWORD("for", token_ticks::FOR),
    CREATE_KEYWORD("while", token_ticks::WHILE),
    CREATE_KEYWORD("import", token_ticks::IMPORT),
    CREATE_KEYWORD("goto", token_ticks::GOTO),
    CREATE_KEYWORD("del", token_ticks::DEL),
    CREATE_KEYWORD("assert", token_ticks::ASSERT),
    CREATE_KEYWORD("if", token_ticks::IF),
    CREATE_KEYWORD("class", token_ticks::CLASS),
    CREATE_KEYWORD("func", token_ticks::FUNC),
    CREATE_KEYWORD("and", token_ticks::AND),
    CREATE_KEYWORD("or", token_ticks::OR),
    CREATE_KEYWORD("not", token_ticks::NOT),
    CREATE_KEYWORD("null", token_ticks::NULL_),
    CREATE_KEYWORD("true", token_ticks::TRUE_),
    CREATE_KEYWORD("false", token_ticks::FALSE_),
    CREATE_KEYWORD("break", token_ticks::BREAK),
    CREATE_KEYWORD("continue", token_ticks::CONTINUE) };

#undef CREATE_KEYWORD

token token_lex::lex_english() {
    const char* start = char_ptr;
    do {
        ++char_ptr;
    } while ((is_english(*char_ptr) || isdigit(*char_ptr)) && !end_of_lex());
    size_t len = char_ptr - start;
    for (const auto& keyword : keywords_) {
        if (keyword.len == len && !strncmp(start, keyword.str, len)) {
            // 传入空串是因为能在此被匹配的，都可以用token_ticks表达含义，不需要储存具体信息
            return token { keyword.tick };
        }
    }
    auto tmp = std::string(start, len);
    // 啥关键字都不是，只能是名称了
    return token { token_ticks::NAME,
        compiler_data.const_name.add(std::string(start, len)) };
}

token_ticks token_lex::get_binary_ticks(
    char expected_char, token_ticks expected, token_ticks unexpected) {
    ++char_ptr;
    if (*char_ptr == expected_char) {
        return expected;
    } else {
        --char_ptr;
        return unexpected;
    }
}

void token_lex::check_expected_char(char expected_char) {
    ++char_ptr;
    if (*char_ptr != expected_char) {
        char err_tmp[] = { *char_ptr, '\0' };
        compiler_data.error.send_error_module(error::SyntaxError,
            language::error::syntaxerror_no_expect, err_tmp);
    }
}

token token_lex::lex_others() {
    token result;
    switch (*char_ptr) {
    case '<': {
        result = token { get_binary_ticks(
            '=', token_ticks::LESS_EQUAL, token_ticks::LESS) };
        break;
    }
    case '>': {
        result = token { get_binary_ticks(
            '=', token_ticks::GREATER_EQUAL, token_ticks::GREATER) };
        break;
    }
    case '=': {
        result = token { get_binary_ticks(
            '=', token_ticks::EQUAL, token_ticks::ASSIGN) };
        break;
    }
    case '!': {
        check_expected_char('=');
        result = token { token_ticks::UNEQUAL };
        break;
    }
    case ':': {
        check_expected_char('=');
        result = token { token_ticks::STORE };
        break;
    }
    case '+': {
        result = token { get_binary_ticks(
            '=', token_ticks::SELFADD, token_ticks::ADD) };
        break;
    }
    case '-': {
        result = token { get_binary_ticks(
            '=', token_ticks::SELFSUB, token_ticks::SUB) };
        break;
    }
    case '*': {
        // *比较特殊，有**符号
        if (get_binary_ticks('*', token_ticks::POW, token_ticks::UNKNOWN)
            == token_ticks::POW) {
            // 确认有两个**
            result = token { get_binary_ticks(
                '=', token_ticks::SELFPOW, token_ticks::POW) };
        } else {
            // 只有一个*
            result = token { get_binary_ticks(
                '=', token_ticks::SELFMUL, token_ticks::MUL) };
        }
        break;
    }
    case '/': {
        // /符号是最特殊的，因为有//符号和/*符号
        if (get_binary_ticks('/', token_ticks::ZDIV, token_ticks::UNKNOWN)
            == token_ticks::ZDIV) {
            // 确认有两个//
            result = token { get_binary_ticks(
                '=', token_ticks::SELFZDIV, token_ticks::ZDIV) };
        } else {
            // 只有一个/
            if (get_binary_ticks('*', token_ticks::MUL, token_ticks::UNKNOWN)
                == token_ticks::MUL) {
                // 说明是/*符号，开启注释

                // 略过当前的*字符
                ++char_ptr;
                for (;;) {
                    if (*char_ptr == '*') {
                        // 遇到*/的开头，可能可以退出,不是也不用退格，反正都是注释里的，没有实际意义
                        ++char_ptr;
                        if (*char_ptr == '/') {
                            break;
                        } else if (*char_ptr == '\n') {
                            // 跨行注释也需要更新行号
                            compiler_data.error.add_line();
                        }
                    }
                    if (end_of_lex()) {
                        // 注释未结尾，报错
                        compiler_data.error.send_error_module(
                            error::SyntaxError,
                            language::error::syntaxerror_lexanno);
                    }
                    ++char_ptr;
                }
            } else {
                result = token { get_binary_ticks(
                    '=', token_ticks::SELFDIV, token_ticks::DIV) };
            }
        }
        break;
    }
    case '%': {
        result = token { get_binary_ticks(
            '=', token_ticks::SELFMOD, token_ticks::MOD) };
        break;
    }
    /* 以下的这些括号需要进行括号匹配进行验证 */
    case '(': {
        result = token { token_ticks::LEFT_SMALL_BRACE };
        check_brace.push('(');
        break;
    }
    case ')': {
        result = token { token_ticks::RIGHT_SMALL_BRACE };
        if (check_brace.empty() || check_brace.top() != '(') {
            compiler_data.error.send_error_module(error::SyntaxError,
                language::error::syntaxerror_no_expect, ")");
        }
        check_brace.pop();
        break;
    }
    case '[': {
        result = token { token_ticks::LEFT_MID_BRACE };
        check_brace.push('[');
        break;
    }
    case ']': {
        result = token { token_ticks::RIGHT_MID_BRACE };
        if (check_brace.empty() || check_brace.top() != '[') {
            compiler_data.error.send_error_module(error::SyntaxError,
                language::error::syntaxerror_no_expect, "]");
        }
        check_brace.pop();
        break;
    }
    case '{': {
        result = token { token_ticks::LEFT_BIG_BRACE };
        check_brace.push('{');
        break;
    }
    case '}': {
        result = token { token_ticks::RIGHT_BIG_BRACE };
        if (check_brace.empty() || check_brace.top() != '{') {
            compiler_data.error.send_error_module(error::SyntaxError,
                language::error::syntaxerror_no_expect, "}");
        }
        check_brace.pop();
        break;
    }
    case ',': {
        result = token { token_ticks::COMMA };
        break;
    }
    case '.': {
        result = token { token_ticks::POINT };
        break;
    }
    default: {
        // 如果一个字符都没有匹配到，报错
        char error_tmp[2] = { *char_ptr, '\0' };
        compiler_data.error.send_error_module(error::SyntaxError,
            language::error::syntaxerror_no_expect, error_tmp);
    }
    }
    // 跳过当前字符
    ++char_ptr;
    return result;
}

void token_lex::unget_token(token token_data) {
    if (token_data.tick == token_ticks::END_OF_LINE) {
        compiler_data.error.sub_line();
    }
    back_token = token_data;
    has_back_token = true;
}

token token_lex::get_token() {
    if (has_back_token) {
        has_back_token = false;
        if (back_token.tick == token_ticks::END_OF_LINE) {
            compiler_data.error.add_line();
        }
        return back_token;
    }
    if (*char_ptr == '#') {
        /*忽略注释*/
        while (!end_of_lex()) {
            // 只要不读完文件或本行，就往下读
            ++char_ptr;
        }
    }
    if (*char_ptr == '\n') {
        // 加一行
        compiler_data.error.add_line();
        ++char_ptr;
        return token { token_ticks::END_OF_LINE };
    }
    if (*char_ptr == '\0') {
        // 解析结束
        return token { token_ticks::END_OF_TOKENS };
    }
    while (*char_ptr == ' ' || *char_ptr == '\t') {
        /*略过空白符和制表符*/
        ++char_ptr;
    }
    if (*char_ptr == '\'' || *char_ptr == '"') {
        /*解析字符串*/
        return lex_string();
    }
    if (isdigit(*char_ptr)) {
        /*解析数字*/
        return lex_int_float();
    }
    if (is_english(*char_ptr)) {
        /*英文字符，有多种可能，累计直到匹配到关键字（关键字）或者不为英文字符（名称）*/
        return lex_english();
    }
    // 各种符号的解析，不满足会报错
    return lex_others();
}

token_lex::token_lex(compiler_public_data& compiler_data)
    : compiler_data(compiler_data) {
}

void token_lex::set_code(const std::string& code) {
    char_ptr = code.c_str();
}

token_lex::~token_lex() {
    // 最后判断括号栈是否为空，如果不为空，说明括号未完全匹配，报错
    if (!check_brace.empty()) {
        char error_tmp[] = { check_brace.top(), '\0' };
        compiler_data.error.send_error_module(error::SyntaxError,
            language::error::syntaxerror_unmatched_char, error_tmp);
    }
    compiler_data.error.reset_line();
}
}
