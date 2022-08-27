/**
 * @file test_filesys.cpp
 * @brief 测试filesys模块
 * @date 2022-05-03
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <algorithm>
#include <base/utils/filesys.h>
#include <filesys_tools.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <list>

using namespace trc;

// 测试遍历文件夹的函数
TEST(filesys, listfiles) {
    utils::listfiles listfiles(redefine_path("filesys/listdirs"));
    std::list<std::string> res
        = { "indir1.txt", "indir2.txt", "t1.txt", "t2.txt", "other.txt" };
    while (true) {
        std::filesystem::path str = listfiles.nextitem();
        if (str.empty()) {
            break;
        }
        for (auto i = res.begin(); i != res.end(); ++i) {
            if (*i == str.filename().string()) {
                res.erase(i);
                goto success;
            }
        }
        ASSERT_TRUE(false) << str;
    success:
        continue;
    }
}

// 检查文件是否存在函数
TEST(filesys, file_exists) {
    EXPECT_FALSE(
        utils::file_exists(redefine_path("filesys/file_exists/read.txt")));
    EXPECT_TRUE(
        utils::file_exists(redefine_path("filesys/file_exists/exist1")));
    EXPECT_TRUE(
        utils::file_exists(redefine_path("filesys/file_exists/exist3.txt")));
    EXPECT_TRUE(utils::file_exists(
        redefine_path("filesys/file_exists/longlonglonglongname.txt")));
    EXPECT_FALSE(
        utils::file_exists(redefine_path("filesys/file_exists/exist2")));
    EXPECT_FALSE(utils::file_exists(redefine_path("filesys/file_exists/dir")));
    EXPECT_FALSE(utils::file_exists(
        redefine_path("filesys/file_exists/dir_not_defined")));
}

static void test_readfile(const char* path, const char* expect) {
    std::string filedata;
    utils::readcode(filedata, redefine_path(path));
    EXPECT_STREQ(filedata.c_str(), expect);
}

// 测试读取文件的函数
TEST(filesys, readfile) {
    // 测试读取多行文件
    test_readfile("filesys/readfile/readwithlines.in",
        "\n\n\nreadwithlines\n\nreadwithlines\n");
    // 测试读取空文件
    test_readfile("filesys/readfile/readempty.in", "");
    // 测试读取英文文件
    test_readfile("filesys/readfile/readenglish.in",
        "abcd\nefgh\nijkl\nmnop\nqrst\nuvwx\nyz\n");
    // 测试读取中文文件
    test_readfile("filesys/readfile/readchinese.in",
        "生活就像海洋，只有意志坚强的人才能到达彼岸\n");
    std::string filedata;
    // 测试读取不存在文件
    EXPECT_EQ(1,
        utils::readcode_with_code(
            filedata, redefine_path("filesys/readfile/failtoread")));
    // 测试成功读取文件
    filedata.clear();
    EXPECT_EQ(0,
        utils::readcode_with_code(
            filedata, redefine_path("filesys/readfile/readenglish.in")));
    // 测试读取超大文件
    std::string expect_big_data;
    char basestr[] = "pppppppppppp\n";
    expect_big_data.reserve(sizeof(basestr) * 10240);
    for (int i = 0; i < 10240; ++i) {
        expect_big_data += basestr;
    }
    test_readfile("filesys/readfile/readlonglong.in", expect_big_data.c_str());
}
