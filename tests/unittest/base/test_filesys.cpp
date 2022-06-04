/**
 * @file test_filesys.cpp
 * @brief 测试filesys模块
 * @date 2022-05-03
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "base/trcdef.h"
#include "base/utils/filesys.h"
#include <algorithm>
#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

using namespace trc;

static std::string redefine_path(const std::string& p) {
    return "../tests/unittest/testdata/" + p;
}

static void check_items(
    const std::vector<fs::path>& raw_items,
    const std::vector<fs::path>& expect_result) {
    for (const auto& raw_item : raw_items) {
        bool flag = false;
        fs::path filename = raw_item.filename();
        for (const auto& j : expect_result) {
            if (j == filename) {
                flag = true;
                break;
            }
        }
        EXPECT_TRUE(flag);
    }
    EXPECT_EQ(raw_items.size(), expect_result.size());
}

// 测试遍历文件夹的函数
TEST(filesys, listfiles) {
    std::vector<fs::path> filelist, dirlist;
    utils::listfiles(redefine_path("filesys/listdirs"),
        filelist, dirlist,
        [](const std::filesystem::path& path) -> bool {
            if (path.stem().string().find('t')
                != std::string::npos) {
                return true;
            } else {
                return false;
            }
        });
    check_items(
        filelist, { "t1.txt", "t2.txt", "other.txt" });
}

// 检查文件是否存在函数
TEST(filesys, file_exists) {
    EXPECT_FALSE(utils::file_exists(
        redefine_path("filesys/file_exists/read.txt")));
    EXPECT_TRUE(utils::file_exists(
        redefine_path("filesys/file_exists/exist1")));
    EXPECT_TRUE(utils::file_exists(
        redefine_path("filesys/file_exists/exist3.txt")));
    EXPECT_TRUE(utils::file_exists(redefine_path(
        "filesys/file_exists/longlonglonglongname.txt")));
    EXPECT_FALSE(utils::file_exists(
        redefine_path("filesys/file_exists/exist2")));
    EXPECT_FALSE(utils::file_exists(
        redefine_path("filesys/file_exists/dir")));
    EXPECT_FALSE(utils::file_exists(redefine_path(
        "filesys/file_exists/dir_not_defined")));
}

static std::string filedata;

static void test_readfile(
    const char* path, const char* expect) {
    utils::readcode(filedata, redefine_path(path));
    EXPECT_STREQ(filedata.c_str(), expect);
    filedata.clear();
}

// 测试读取文件的函数
TEST(filesys, readfile) {
    // 测试读取多行文件
    test_readfile("filesys/readfile/readwithlines.txt",
        "\n\n\nreadwithlines\n\nreadwithlines\n");
    // 测试读取空文件
    test_readfile("filesys/readfile/readempty.txt", "");
    // 测试读取英文文件
    test_readfile("filesys/readfile/readenglish.txt",
        "abcd\nefgh\nijkl\nmnop\nqrst\nuvwx\nyz\n");
    // 测试读取中文文件
    test_readfile("filesys/readfile/readchinese.txt",
        "生活就像海洋，只有意志坚强的人才能到达彼岸\n");
    filedata.clear();
    EXPECT_EQ(1,
        utils::readcode_with_code(filedata,
            redefine_path("filesys/readfile/failtoread")));
    filedata.clear();
    EXPECT_EQ(0,
        utils::readcode_with_code(filedata,
            redefine_path(
                "filesys/readfile/readenglish.txt")));
}
