#include "../testdata/autodll/create_dll.h"
#include <base/autodll.h>
#include <filesys_tools.h>
#include <gtest/gtest.h>

using namespace trc;

// 测试能否从动态链接库中读取函数并执行
TEST(autodll, dllfunc) {
    char* name = autodll::redirect_to_platform(
        redefine_path("autodll/create_dll"));
    autodll::dll_t test_ = autodll::dllopen(name);
    delete[] name;
    auto add_
        = (add_func)autodll::dllfuncload(test_, "add");
    EXPECT_EQ(3, add_(1, 2));
    EXPECT_EQ(9, add_(2, 7));
    auto return_ = (return_func)autodll::dllfuncload(
        test_, "return_");
    EXPECT_STREQ("ppppp", return_("ppppp"));
    autodll::dllfree(test_);
}
