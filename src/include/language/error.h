#include "dll.h"

namespace language::error {
TRC_language_api extern const char* error_map[];
TRC_language_api extern const char* error_from;
TRC_language_api extern const char* error_in_line;

// 这些是常用的在不同场景下的报错信息，也可以自己定义

TRC_language_api extern const char* nameerror;
TRC_language_api extern const char* openfileerror;
TRC_language_api extern const char* versionerror;
TRC_language_api extern const char* memoryerror;
TRC_language_api extern const char* argumenterror;
TRC_language_api extern const char* modulenotfounderror;
TRC_language_api extern const char* keyerror;
TRC_language_api extern const char* indexerror;
TRC_language_api extern const char* valueerror;

// 对数字和浮点数的解析
TRC_language_api extern const char* syntaxerror_int;
// 对字符串的解析
TRC_language_api extern const char* syntaxerror_lexstring;
// 对注释的解析
TRC_language_api extern const char* syntaxerror_lexanno;
// 遇到不期待字符时报的错
TRC_language_api extern const char* syntaxerror_no_expect;
// 未遇到期待字符，表示期待的字符
TRC_language_api extern const char* syntaxerror_expect;

// 断言的默认报错信息
TRC_language_api extern const char* asserterror_default;
// 断言的用户定义报错信息
TRC_language_api extern const char* asserterror_user;

TRC_language_api extern const char* zerodiverror;

TRC_language_api extern const char* dll_open_err;

TRC_language_api extern const char* noreach;

TRC_language_api extern const char* magic_value_error;
}
