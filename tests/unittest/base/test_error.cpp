#include <base/Error.h>
#include <cstdarg>
#include <gtest/gtest.h>
#include <string>

using namespace trc;

static void test_make_error_msg(const char* right_msg, int error_name, ...) {
    va_list ap;
    va_start(ap, error_name);
    char* program_result = error::make_error_msg(error_name, ap);
    EXPECT_STREQ(right_msg, program_result);
    delete[] program_result;
    va_end(ap);
}

TEST(error, make_error_msg) {
    // 测试插入字符串
    test_make_error_msg("SystemError:o88p", error::SystemError, "o%p", "88");
    test_make_error_msg("SystemError:ppp", error::SystemError, "ppp");
    test_make_error_msg(
        "SystemError:piq", error::SystemError, "%%%", "p", "i", "q");
    // 测试内存
    test_make_error_msg("SystemError:"
                        "ppppppppppppppppppp77ppppppppppooooooooooooo",
        error::SystemError, "%pp%%o", "ppppppppppppppppp", "77pppppppppp",
        "oooooooooooo");
    // 测试插入字符
    test_make_error_msg("SystemError:hope to pass", error::SystemError,
        "% %% %", "hope", "t", "o", "pass");
}
