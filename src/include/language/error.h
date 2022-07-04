#include <language/library.h>

namespace language::error {
TRC_language_c_api const char* error_map[];
TRC_language_c_api const char* error_from;
TRC_language_c_api const char* error_in_line;

// 这些是常用的在不同场景下的报错信息，也可以自己定义

TRC_language_c_api const char* nameerror;
TRC_language_c_api const char* openfileerror;
TRC_language_c_api const char* versionerror;
TRC_language_c_api const char* memoryerror;
TRC_language_c_api const char* argumenterror;
TRC_language_c_api const char* modulenotfounderror;
TRC_language_c_api const char* keyerror;
TRC_language_c_api const char* indexerror;
TRC_language_c_api const char* valueerror;

// 对数字和浮点数的解析
TRC_language_c_api const char* syntaxerror_int;
// 对字符串的解析
TRC_language_c_api const char* syntaxerror_lexstring;
// 对注释的解析
TRC_language_c_api const char* syntaxerror_lexanno;
// 遇到不期待字符时报的错
TRC_language_c_api const char* syntaxerror_no_expect;
// 未遇到期待字符，表示期待的字符
TRC_language_c_api const char* syntaxerror_expect;
// 未定义的转义符被读取到
TRC_language_c_api const char* syntaxerror_escape_char;
// 不明确的语法错误
TRC_language_c_api const char* syntaxerror;
// 无法匹配的符号
TRC_language_c_api const char* syntaxerror_unmatched_char;

// 断言的默认报错信息
TRC_language_c_api const char* asserterror_default;
// 断言的用户定义报错信息
TRC_language_c_api const char* asserterror_user;

TRC_language_c_api const char* zerodiverror;

TRC_language_c_api const char* dll_open_err;

TRC_language_c_api const char* noreach;

TRC_language_c_api const char* magic_value_error;
}
