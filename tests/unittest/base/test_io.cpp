/**
 * 测试自己封装的io库
 */

#include "base/io.hpp"
#include "filesys_tools.h"
#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace trc;

static void check_readstr(const char* path,
    const std::vector<const char*>& expectstr) {
    FILE* file = fopen_with_check(redefine_path(path).c_str(), "r");
    char* raw_str;
    // -1是为了避开最后一个false(eof)值
    for (size_t i = 0, n = expectstr.size(); i < n - 1;
         ++i) {
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
    check_readstr(
        "io/readstr1.in", { "abcdef", "opiuy", "pploi" });
    // 测试结尾不为换行符的文件
    check_readstr("io/readstr_eof.in", { "io", "ploip" });
    // 测试空文件读入
    check_readstr("io/readstr_empty.in", { "" });
    // 测试长字符串读入
    check_readstr("io/readstr_long.in",
        { std::string(120, 'u').c_str(),
            std::string(120, 'v').c_str() });
}

// 整型文件快读
TEST(io, fast_int_read) {
    // 测试读取空文件
    bool isright = true;
    FILE*file = fopen_with_check(redefine_path("io/fastread_empty.in").c_str(), "r");
    EXPECT_EQ(0, io::fast_int_read<int>(file, isright));
    EXPECT_FALSE(isright);
    fclose(file);
    // 测试读取各种类型的整型
    isright = true;
    file = fopen_with_check(redefine_path("io/fastread.in").c_str(), "r");
    std::vector<int> expect_res = {
        789,456,123,0,0,14,-7,-2369
    };
    for(auto i : expect_res) {
        EXPECT_EQ(i, io::fast_int_read<int>(file, isright));
        EXPECT_TRUE(isright);
    }
    fclose(file);
}

// 整型文件快输
TEST(io, fast_int_write) {
    // 先通过写入文件的方式，然后再通过字符串比较的方式来校验代码是否正确
    FILE* file = fopen_with_check(redefine_path("io/fastwrite.out").c_str(), "w");
    io::fast_int_write(10, file);
    fputc(' ', file);
    io::fast_int_write(-10, file);
    io::fast_int_write(0, file);
    fputc(' ', file);
    io::fast_int_write(1000000, file);
    fclose(file);

    file = fopen_with_check(redefine_path("io/fastwrite.out").c_str(), "r");
    char* res;
    io::readstr(res, file);
    EXPECT_STREQ(res, "10 -100 1000000");
    free(res);
}
