export module language;

export namespace language {
/**
 * @brief 地域化初始化设置
 */
void locale_init();

namespace help {
    extern const char* help_msg;
}
namespace trc {
    extern const char* mode_not_found;
}
namespace tdb {
    extern const char* var;
    extern const char* not_defined;
    extern const char* start_tip;
    extern const char* instruction;
}

namespace error {
    extern const char* error_map[];
    extern const char* error_from;
    extern const char* error_in_line;

    // 这些是常用的在不同场景下的报错信息，也可以自己定义

    extern const char* nameerror;
    extern const char* openfileerror;
    extern const char* versionerror;
    extern const char* memoryerror;
    extern const char* argumenterror;
    extern const char* modulenotfounderror;
    extern const char* keyerror;
    extern const char* indexerror;
    extern const char* valueerror;
    extern const char* operatorerror;
    extern const char* funcredefinederror;
    extern const char* varredefinederror;

    // 对数字和浮点数的解析
    extern const char* syntaxerror_int;
    // 对字符串的解析
    extern const char* syntaxerror_lexstring;
    // 对注释的解析
    extern const char* syntaxerror_lexanno;
    // 遇到不期待字符时报的错
    extern const char* syntaxerror_no_expect;
    // 未遇到期待字符，表示期待的字符
    extern const char* syntaxerror_expect;
    // 未定义的转义符被读取到
    extern const char* syntaxerror_escape_char;
    // 不明确的语法错误
    extern const char* syntaxerror;
    // 无法匹配的符号
    extern const char* syntaxerror_unmatched_char;

    // 断言的默认报错信息
    extern const char* asserterror_default;
    // 断言的用户定义报错信息
    extern const char* asserterror_user;

    extern const char* zerodiverror;

    extern const char* dll_open_err;

    extern const char* noreach;

    extern const char* magic_value_error;
}
}
