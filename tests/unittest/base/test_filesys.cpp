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

#define redefine_path(p) "../tests/unittest/testdata/" p

using namespace trc;

static void check_items(
    const std::vector<fs::path>& raw_items,
    const std::vector<fs::path>& expect_result) {
    for (int i = 0, n = raw_items.size(); i < n; ++i) {
        bool flag = false;
        fs::path filename = raw_items[i].filename();
        for (int j = 0, nj = expect_result.size(); j < nj;
             ++j) {
            if (expect_result[j] == filename) {
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
            if (path.stem().string().find("t")
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

// 测试读取文件的函数
TEST(filesys, readfile) {
    // 测试读取空文件
    std::string filedata;
    utils::readcode(filedata,
        redefine_path("filesys/readfile/readempty.txt"));
    EXPECT_EQ(filedata, "");
    // 测试读取多行文件
    filedata.clear();
    utils::readcode(filedata,
        redefine_path(
            "filesys/readfile/readwithlines.txt"));
    EXPECT_EQ(
        filedata, "\n\n\nreadwithlines\n\nreadwithlines\n");
}
