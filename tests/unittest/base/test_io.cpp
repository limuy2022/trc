/**
 * 测试自己封装的io库
 */

#include <base/io.hpp>
#include <cstdio>
#include <cstdlib>
#include <filesys_tools.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace trc;

static void check_readstr(
    const char* path, const std::vector<const char*>& expectstr) {
    FILE* file = fopen_with_check(redefine_path(path).c_str(), "r");
    char* raw_str;
    // -1是为了避开最后一个false(eof)值
    for (size_t i = 0, n = expectstr.size(); i < n - 1; ++i) {
        EXPECT_TRUE(io::readstr(raw_str, file));
        EXPECT_STREQ(raw_str, expectstr[i]);
        free(raw_str);
    }
    io::readstr(raw_str, file);
    EXPECT_STREQ(raw_str, expectstr.back());
    free(raw_str);
    fclose(file);
}

// 测试字符串读入
TEST(io, readstr) {
    // 测试普通正常多行文件
    check_readstr("io/readstr1.in", { "abcdef", "opiuy", "pploi" });
    // 测试结尾不为换行符的文件
    check_readstr("io/readstr_eof.in", { "io", "ploip" });
    // 测试空文件读入
    check_readstr("io/readstr_empty.in", { "" });
    // 测试长字符串读入
    check_readstr("io/readstr_long.in",
        { std::string(120, 'u').c_str(), std::string(120, 'v').c_str() });
}
