#include <base/Error.h>
#include <cstdarg>
#include <cstdlib>
#include <gtest/gtest.h>
#include <string>

static void test_make_error_msg(const char* right_msg, int error_name, ...) {
    va_list ap;
    va_start(ap, error_name);
    char* program_result = trc::error::make_error_msg(error_name, ap);
    EXPECT_STREQ(right_msg, program_result);
    free(program_result);
    va_end(ap);
}

TEST(error, make_error_msg) {
    test_make_error_msg(
        "SystemError:o88p", trc::error::SystemError, "o%p", "88");
    test_make_error_msg("SystemError:ppp", trc::error::SystemError, "ppp");
    test_make_error_msg(
        "SystemError:piq", trc::error::SystemError, "%%%", "p", "i", "q");
    // 测试内存
    test_make_error_msg("SystemError:"
                        "ppppppppppppppppppp77ppppppppppooooooooooooo",
        trc::error::SystemError, "%pp%%o", "ppppppppppppppppp", "77pppppppppp",
        "oooooooooooo");
}
