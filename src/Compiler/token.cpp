#include <Compiler/Compiler.h>
#include <Compiler/pri_compiler.hpp>
#include <TVM/types/trc_flong.h>
#include <TVM/types/trc_long.h>
#include <base/Error.h>
#include <base/utils/data.hpp>
#include <language/error.h>
#include <string>
#include <vector>

// todo:完成代码检查功能
namespace trc::compiler {
void token_lex::lex_string(token* result) {
    char string_begin = *char_ptr;
    result->tick = token_ticks::STRING_VALUE;
    // 略过当前"符号
    get_next_char();
    while (*char_ptr != string_begin) {
        if (*char_ptr == '\\') {
            // 转义符
            // 读出真实符号并匹配转为真实符号
            get_next_char();
            switch (*char_ptr) {
            case 'r': {
                result->data += '\r';
                break;
            }
            case 'b': {
                result->data += '\b';
                break;
            }
            case 'n': {
                result->data += '\n';
                break;
            }
            case '\'': {
                result->data += '\'';
                break;
            }
            case '"': {
                result->data += '"';
                break;
            }
            case 't': {
                result->data += '\t';
                break;
            }
            case '\\': {
                result->data += '\\';
                break;
            }
            case '0': {
                result->data += '\0';
                break;
            }
            case 'a': {
                result->data += '\a';
                break;
            }
            case 'f': {
                result->data += '\f';
                break;
            }
            case 'v': {
                result->data += '\v';
                break;
            }
            default: {
                error_->send_error_module(error::SyntaxError,
                    language::error::syntaxerror_escape_char);
            }
            }
        } else {
            result->data += *char_ptr;
        }
        // 读入下一符号
        get_next_char();
        if (end_of_lex()) {
            // 读到文件末尾了，说明字符串解析错误
            error_->send_error_module(
                error::SyntaxError, language::error::syntaxerror_lexstring);
        }
    }
    // 略过字符串终结符
    get_next_char();
}

void token_lex::lex_int_float(token* result) {
    result->tick = token_ticks::INT_VALUE;
    while (!end_of_lex()) {
        if (!isdigit(*char_ptr)) {
            if (*char_ptr == '.') {
                // 小数点，开启调整类型为浮点数
                result->tick = token_ticks::FLOAT_VALUE;
            } else if (*char_ptr == '_') {
                // 忽略123_456中间的下划线
                get_next_char();
                continue;
            } else {
                return;
            }
        }
        result->data += *char_ptr;
        get_next_char();
    }
    // 尝试纠正为长整型和长浮点型
    switch (result->tick) {
    case token_ticks::FLOAT_VALUE: {
        if (result->data.length() - 1 > FLOAT_LONGFLOAT_LINE) {
            result->tick = token_ticks::LONG_FLOAT_VALUE;
        }
        break;
    }
    case token_ticks::INT_VALUE: {
        if (result->data.length() > INT_LONGINT_LINE) {
            result->tick = token_ticks::LONG_INT_VALUE;
        }
        break;
    }
    default: {
        NOREACH;
    }
    }
}

bool token_lex::end_of_lex() const noexcept {
    return *char_ptr == '\n' || *char_ptr == '\0';
}

std::pair<const char*, token_ticks> keywords_[] = { { "for", token_ticks::FOR },
    { "while", token_ticks::WHILE }, { "import", token_ticks::IMPORT },
    { "goto", token_ticks::GOTO }, { "del", token_ticks::DEL },
    { "assert", token_ticks::ASSERT }, { "if", token_ticks::IF },
    { "class", token_ticks::CLASS }, { "func", token_ticks::FUNC },
    { "and", token_ticks::AND }, { "or", token_ticks::OR },
    { "not", token_ticks::NOT }, { "null", token_ticks::NULL_ },
    { "true", token_ticks::TRUE }, { "false", token_ticks::FALSE } };

void token_lex::lex_english(token* result) {
    result->data += *char_ptr;
    get_next_char();
    for (;;) {
        if (!(is_english(*char_ptr) || isdigit(*char_ptr)) || end_of_lex()) {
            // 不满足字母，数字，下划线
            break;
        }
        result->data += *char_ptr;
        get_next_char();
    }
    for (size_t i = 0, n = utils::sizeof_static_array(keywords_); i < n; ++i) {
        if (result->data == keywords_[i].first) {
            // 注：传入空串的原因是能在此被匹配的，都可以用token_ticks表达含义，不需要储存具体信息
            result->data.clear();
            result->tick = keywords_[i].second;
            return;
        }
    }
    // 啥关键字都不是，只能是名称了
    result->tick = token_ticks::NAME;
}

token_ticks token_lex::get_binary_ticks(
    char expected_char, token_ticks expected, token_ticks unexpected) {
    get_next_char();
    if (*char_ptr == expected_char) {
        return expected;
    } else {
        to_back_char();
        return unexpected;
    }
}

void token_lex::check_expected_char(char expected_char) {
    get_next_char();
    if (*char_ptr != expected_char) {
        char err_tmp[] = { *char_ptr, '\0' };
        error_->send_error_module(error::SyntaxError,
            language::error::syntaxerror_no_expect, err_tmp);
    }
}

void token_lex::lex_others(token* result) {
    switch (*char_ptr) {
    case '<': {
        result->tick
            = get_binary_ticks('=', token_ticks::LESS_EQUAL, token_ticks::LESS);
        break;
    }
    case '>': {
        result->tick = get_binary_ticks(
            '=', token_ticks::GREATER_EQUAL, token_ticks::GREATER);
        break;
    }
    case '=': {
        result->tick
            = get_binary_ticks('=', token_ticks::EQUAL, token_ticks::ASSIGN);
        break;
    }
    case '!': {
        check_expected_char('=');
        result->tick = token_ticks::UNEQUAL;
        break;
    }
    case ':': {
        check_expected_char('=');
        result->tick = token_ticks::STORE;
        break;
    }
    case '+': {
        result->tick
            = get_binary_ticks('=', token_ticks::SELFADD, token_ticks::ADD);
        break;
    }
    case '-': {
        result->tick
            = get_binary_ticks('=', token_ticks::SELFSUB, token_ticks::SUB);
        break;
    }
    case '*': {
        // *比较特殊，有**符号
        if (get_binary_ticks('*', token_ticks::POW, token_ticks::UNKNOWN)
            == token_ticks::POW) {
            // 确认有两个**
            result->tick
                = get_binary_ticks('=', token_ticks::SELFPOW, token_ticks::POW);
        } else {
            // 只有一个*
            result->tick
                = get_binary_ticks('=', token_ticks::SELFMUL, token_ticks::MUL);
        }
        break;
    }
    case '/': {
        // /符号是最特殊的，因为有//符号和/*符号
        if (get_binary_ticks('/', token_ticks::ZDIV, token_ticks::UNKNOWN)
            == token_ticks::ZDIV) {
            // 确认有两个//
            result->tick = get_binary_ticks(
                '=', token_ticks::SELFZDIV, token_ticks::ZDIV);
        } else {
            // 只有一个/
            if (get_binary_ticks('*', token_ticks::MUL, token_ticks::UNKNOWN)
                == token_ticks::MUL) {
                // 说明是/*符号，开启注释

                // 略过当前的*字符
                get_next_char();
                for (;;) {
                    if (*char_ptr == '*') {
                        // 遇到*/的开头，可能可以退出,不是也不用退格，反正都是注释里的，没有实际意义
                        get_next_char();
                        if (*char_ptr == '/') {
                            break;
                        } else if (*char_ptr == '\n') {
                            // 跨行注释也需要更新行号
                            error_->line++;
                        }
                    }
                    if (end_of_lex()) {
                        // 注释未结尾，报错
                        error_->send_error_module(error::SyntaxError,
                            language::error::syntaxerror_lexanno);
                    }
                    get_next_char();
                }
            } else {
                result->tick = get_binary_ticks(
                    '=', token_ticks::SELFDIV, token_ticks::DIV);
            }
        }
        break;
    }
    case '%': {
        result->tick
            = get_binary_ticks('=', token_ticks::SELFMOD, token_ticks::MOD);
        break;
    }
    /* 以下的这些括号需要进行括号匹配进行验证 */
    case '(': {
        result->tick = token_ticks::LEFT_SMALL_BRACE;
        check_brace.push('(');
        break;
    }
    case ')': {
        result->tick = token_ticks::RIGHT_SMALL_BRACE;
        if (check_brace.top() != '(') {
            error_->send_error_module(error::SyntaxError,
                language::error::syntaxerror_no_expect, ")");
        }
        check_brace.pop();
        break;
    }
    case '[': {
        result->tick = token_ticks::LEFT_MID_BRACE;
        check_brace.push('[');
        break;
    }
    case ']': {
        result->tick = token_ticks::RIGHT_MID_BRACE;
        if (check_brace.top() != '[') {
            error_->send_error_module(error::SyntaxError,
                language::error::syntaxerror_no_expect, "]");
        }
        check_brace.pop();
        break;
    }
    case '{': {
        result->tick = token_ticks::LEFT_BIG_BRACE;
        check_brace.push('{');
        break;
    }
    case '}': {
        result->tick = token_ticks::RIGHT_BIG_BRACE;
        if (check_brace.top() != '{') {
            error_->send_error_module(error::SyntaxError,
                language::error::syntaxerror_no_expect, "}");
        }
        check_brace.pop();
        break;
    }
    case ',': {
        result->tick = token_ticks::COMMA;
        break;
    }
    case '.': {
        result->tick = token_ticks::POINT;
        break;
    }
    default: {
        // 如果一个字符都没有匹配到，报错
        char error_tmp[2] = { *char_ptr, '\0' };
        error_->send_error_module(error::SyntaxError,
            language::error::syntaxerror_no_expect, error_tmp);
    }
    }
    // 跳过当前字符
    get_next_char();
}

void token_lex::unget_token(token* token_data) {
    back_token = token_data;
    is_unget = true;
}

token* token_lex::get_token() {
    if (is_unget) {
        is_unget = false;
        return back_token;
    }
    auto* result = new token;
    if (*char_ptr == '\n') {
        // 加一行
        error_->line++;
        result->tick = token_ticks::END_OF_LINE;
        get_next_char();
        return result;
    }
    if (*char_ptr == '\0') {
        // 解析结束
        result->tick = token_ticks::END_OF_TOKENS;
        return result;
    }
    while (*char_ptr == ' ' || *char_ptr == '\t') {
        /*略过空白符和制表符*/
        get_next_char();
    }
    while (*char_ptr == '#') {
        /*忽略注释*/
        while (!end_of_lex()) {
            // 只要不读完文件或本行，就往下读
            get_next_char();
        }
    }
    if (*char_ptr == '\'' || *char_ptr == '"') {
        /*解析字符串*/
        lex_string(result);
        return result;
    }
    if (isdigit(*char_ptr)) {
        /*解析数字*/
        lex_int_float(result);
        return result;
    }
    if (is_english(*char_ptr)) {
        /*英文字符，有多种可能，累计直到匹配到关键字（关键字）或者不为英文字符（名称）*/
        lex_english(result);
        return result;
    }
    // 各种符号的解析，不满足会报错
    lex_others(result);
    return result;
}

token_lex::token_lex(const std::string& code, compiler_error* error_)
    : error_(error_)
    , rawcode(code)
    , char_ptr(rawcode.c_str()) {
}

token_lex::~token_lex() {
    error_->line = 0;
    // 最后判断括号栈是否为空，如果不为空，说明括号未完全匹配，报错
    if (!check_brace.empty()) {
        char tmpstr[2] = {check_brace.top(),'\0'};
        error_->send_error_module(error::SyntaxError,
            language::error::syntaxerror_unmatched_char, tmpstr);
    }
}

void token_lex::get_next_char() noexcept {
    char_ptr++;
}

void token_lex::to_back_char() noexcept {
    char_ptr--;
}
}
