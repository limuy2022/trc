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
#include <gtest/gtest.h>

#define redefine_path(p) "../tests/unittest/" p

using namespace trc;

static bool check_items(
    const vecs& raw_items, const vecs& expect_result) {
}

TEST(filesys, listfiles) {
}

// 检查文件是否存在函数
TEST(filesys, file_exists) {
    EXPECT_TRUE(utils::file_exists(
        redefine_path("testdata/filesys/read.txt")));
    EXPECT_TRUE(utils::file_exists(redefine_path(
        "testdata/filesys/listdir_test/listdir_2.txt")));
    EXPECT_FALSE(utils::file_exists(
        redefine_path("test_data/a.txt")));
    EXPECT_FALSE(
        utils::file_exists(redefine_path("test_data/")));
    EXPECT_FALSE(
        utils::file_exists(redefine_path("testdata/")));
    EXPECT_FALSE(utils::file_exists(redefine_path("testdata")));
}
