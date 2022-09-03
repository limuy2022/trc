#include <Compiler/Compiler.h>
#include <Compiler/pri_compiler.hpp>
#include <Compiler/token.h>
#include <TVM/types/trc_flong.h>
#include <TVM/types/trc_long.h>
#include <base/Error.h>
#include <language/error.h>
#include <string>
#include <vector>

// todo:完成代码检查功能
namespace trc::compiler {
token* token_lex::lex_string() {
    // 略过当前"符号
    compiler_data.string_size++;
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
    ptrdiff_t backup = str_len;
    auto* str = new token(token_ticks::STRING_VALUE);
    str->set_size(str_len);
    // 使用自定义的函数方便处理换行符，消除字符的移动
    for (char *i = const_cast<char*>(start), *dataptr = str->data;
         i != char_ptr; ++i, ++dataptr) {
        if (*i == '\\') {
            // 转义符
            // 减少的字符数
            str_len--;
            ++i;
            // 读出真实符号并匹配转为真实符号
            switch (*i) {
            case 'r': {
                *dataptr = '\r';
                break;
            }
            case 'b': {
                *dataptr = '\b';
                break;
            }
            case 'n': {
                *dataptr = '\n';
                break;
            }
            case '\'': {
                *dataptr = '\'';
                break;
            }
            case '"': {
                *dataptr = '"';
                break;
            }
            case 't': {
                *dataptr = '\t';
                break;
            }
            case '\\': {
                *dataptr = '\\';
                break;
            }
            case '0': {
                *dataptr = '\0';
                break;
            }
            case 'a': {
                *dataptr = '\a';
                break;
            }
            case 'f': {
                *dataptr = '\f';
                break;
            }
            case 'v': {
                *dataptr = '\v';
                break;
            }
            default: {
                compiler_data.error.send_error_module(error::SyntaxError,
                    language::error::syntaxerror_escape_char);
            }
            }
        } else {
            *dataptr = *i;
        }
    }
    // 换行符会减少一个字符，所以此处计算减少多少个字符
    if (backup != str_len) {
        str->set_size(str_len);
    }
    // 略过字符串终结符
    ++char_ptr;
    return str;
}

token* token_lex::lex_int_float() {
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
    auto* result = new token();
    ptrdiff_t res_len = char_ptr - start, backup = res_len;
    result->set_size(res_len);
    for (char *i = const_cast<char*>(start), *dataptr = result->data;
         i != char_ptr; ++i, ++dataptr) {
        if (*i == '_') {
            // 忽略123_456中间的下划线
            res_len--;
            dataptr--;
        } else {
            *dataptr = *i;
        }
    }
    if (res_len != backup) {
        // 长度改变，缩减长度
        result->set_size(res_len);
    }
    // 尝试纠正为长整型和长浮点型
    switch (tick_for_res) {
    case token_ticks::FLOAT_VALUE: {
        if (res_len > FLOAT_LONGFLOAT_LINE) {
            compiler_data.long_float_size++;
            tick_for_res = token_ticks::LONG_FLOAT_VALUE;
        } else {
            compiler_data.float_size++;
        }
        break;
    }
    case token_ticks::INT_VALUE: {
        if (res_len > INT_LONGINT_LINE) {
            compiler_data.long_int_size++;
            tick_for_res = token_ticks::LONG_INT_VALUE;
        } else {
            compiler_data.int_size++;
        }
        break;
    }
    default: {
        NOREACH("Another token tick %d", tick_for_res);
    }
    }
    result->tick = tick_for_res;
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
    CREATE_KEYWORD("false", token_ticks::FALSE_) };

#undef CREATE_KEYWORD

token* token_lex::lex_english() {
    const char* start = char_ptr;
    do {
        ++char_ptr;
    } while ((is_english(*char_ptr) || isdigit(*char_ptr)) && !end_of_lex());
    size_t len = char_ptr - start;
    for (auto& keyword : keywords_) {
        if (keyword.len == len && !strncmp(start, keyword.str, len)) {
            // 传入空串的原因是能在此被匹配的，都可以用token_ticks表达含义，不需要储存具体信息
            return new token(keyword.tick);
        }
    }
    // 啥关键字都不是，只能是名称了
    return new token(token_ticks::NAME, start, len);
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

token* token_lex::lex_others() {
    token* result = nullptr;
    switch (*char_ptr) {
    case '<': {
        result = new token(
            get_binary_ticks('=', token_ticks::LESS_EQUAL, token_ticks::LESS));
        break;
    }
    case '>': {
        result = new token(get_binary_ticks(
            '=', token_ticks::GREATER_EQUAL, token_ticks::GREATER));
        break;
    }
    case '=': {
        result = new token(
            get_binary_ticks('=', token_ticks::EQUAL, token_ticks::ASSIGN));
        break;
    }
    case '!': {
        check_expected_char('=');
        result = new token(token_ticks::UNEQUAL);
        break;
    }
    case ':': {
        check_expected_char('=');
        result = new token(token_ticks::STORE);
        break;
    }
    case '+': {
        result = new token(
            get_binary_ticks('=', token_ticks::SELFADD, token_ticks::ADD));
        break;
    }
    case '-': {
        result = new token(
            get_binary_ticks('=', token_ticks::SELFSUB, token_ticks::SUB));
        break;
    }
    case '*': {
        // *比较特殊，有**符号
        if (get_binary_ticks('*', token_ticks::POW, token_ticks::UNKNOWN)
            == token_ticks::POW) {
            // 确认有两个**
            result = new token(
                get_binary_ticks('=', token_ticks::SELFPOW, token_ticks::POW));
        } else {
            // 只有一个*
            result = new token(
                get_binary_ticks('=', token_ticks::SELFMUL, token_ticks::MUL));
        }
        break;
    }
    case '/': {
        // /符号是最特殊的，因为有//符号和/*符号
        if (get_binary_ticks('/', token_ticks::ZDIV, token_ticks::UNKNOWN)
            == token_ticks::ZDIV) {
            // 确认有两个//
            result = new token(get_binary_ticks(
                '=', token_ticks::SELFZDIV, token_ticks::ZDIV));
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
                            compiler_data.error.line++;
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
                result = new token(get_binary_ticks(
                    '=', token_ticks::SELFDIV, token_ticks::DIV));
            }
        }
        break;
    }
    case '%': {
        result = new token(
            get_binary_ticks('=', token_ticks::SELFMOD, token_ticks::MOD));
        break;
    }
    /* 以下的这些括号需要进行括号匹配进行验证 */
    case '(': {
        result = new token(token_ticks::LEFT_SMALL_BRACE);
        check_brace.push('(');
        break;
    }
    case ')': {
        result = new token(token_ticks::RIGHT_SMALL_BRACE);
        if (check_brace.empty() || check_brace.top() != '(') {
            compiler_data.error.send_error_module(error::SyntaxError,
                language::error::syntaxerror_no_expect, ")");
        }
        check_brace.pop();
        break;
    }
    case '[': {
        result = new token(token_ticks::LEFT_MID_BRACE);
        check_brace.push('[');
        break;
    }
    case ']': {
        result = new token(token_ticks::RIGHT_MID_BRACE);
        if (check_brace.empty() || check_brace.top() != '[') {
            compiler_data.error.send_error_module(error::SyntaxError,
                language::error::syntaxerror_no_expect, "]");
        }
        check_brace.pop();
        break;
    }
    case '{': {
        result = new token(token_ticks::LEFT_BIG_BRACE);
        check_brace.push('{');
        break;
    }
    case '}': {
        result = new token(token_ticks::RIGHT_BIG_BRACE);
        if (check_brace.empty() || check_brace.top() != '{') {
            compiler_data.error.send_error_module(error::SyntaxError,
                language::error::syntaxerror_no_expect, "}");
        }
        check_brace.pop();
        break;
    }
    case ',': {
        result = new token(token_ticks::COMMA);
        break;
    }
    case '.': {
        result = new token(token_ticks::POINT);
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

void token_lex::unget_token(token* token_data) {
    // 必须没有储存token，否则就是出现了bug
    assert(back_token == nullptr);
    back_token = token_data;
}

token* token_lex::get_token() {
    if (back_token != nullptr) {
        token* return_temp = back_token;
        back_token = nullptr;
        return return_temp;
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
        compiler_data.error.line++;
        ++char_ptr;
        return new token(token_ticks::END_OF_LINE);
    }
    if (*char_ptr == '\0') {
        // 解析结束
        return new token(token_ticks::END_OF_TOKENS);
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

token_lex::token_lex(
    const std::string& code, compiler_public_data& compiler_data)
    : compiler_data(compiler_data)
    , rawcode(code)
    , char_ptr(rawcode.c_str()) {
}

token_lex::~token_lex() {
    // 最后判断括号栈是否为空，如果不为空，说明括号未完全匹配，报错
    if (!check_brace.empty()) {
        compiler_data.error.send_error_module(error::SyntaxError,
            language::error::syntaxerror_unmatched_char, check_brace.top());
    }
    compiler_data.error.line = 0;
}

token::~token() {
    free(data);
}

token::token(token_ticks tick, const char* data, size_t len)
    : tick(tick)
    , data((char*)(malloc(sizeof(char) * (len + 1)))) {
    if (data == nullptr) {
        error::send_error(error::MemoryError, language::error::memoryerror);
    }
    strncpy(this->data, data, len);
    this->data[len] = '\0';
}

token::token(token_ticks tick)
    : tick(tick)
    , data(nullptr) {
}

void token::set_size(size_t len) {
    // 第一次申请内存也可以使用，因为当data为nullptr是等价于malloc
    data = (char*)realloc(data, sizeof(char) * (1 + len));
    if (data == nullptr) {
        error::send_error(error::MemoryError, language::error::memoryerror);
    }
    data[len] = '\0';
}
}
