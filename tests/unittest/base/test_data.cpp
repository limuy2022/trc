#include "base/trcdef.h"
#include "base/utils/bytes.h"
#include "base/utils/data.hpp"
#include "base/utils/filesys.h"
#include <deque>
#include <gtest/gtest.h>
#include <list>
#include <map>
#include <vector>

TEST(data, bytes) {
    trc::def::byte_t origin_data[] = { 0x64, 0x34 };
    trc::utils::bytes_order_change(origin_data, 2);
    EXPECT_EQ(origin_data[0], 0x34);
    EXPECT_EQ(origin_data[1], 0x64);
}

TEST(data, inrange) {
    EXPECT_FALSE(trc::utils::inrange(6, 1, 9));
    EXPECT_TRUE(trc::utils::inrange(0, 9, 5));
    EXPECT_TRUE(trc::utils::inrange(-5, -1, -2));
    EXPECT_FALSE(trc::utils::inrange(-9, 0, 1));
}

TEST(data, index_vector) {
    std::list<int> i = { 9, 11, 2, 11, 11 };
    EXPECT_EQ(trc::utils::index_vector(i, 11), 1);

    std::vector<int> u = { 8, 8, 9 };
    EXPECT_EQ(trc::utils::index_vector(u, 8), 0);
    EXPECT_EQ(trc::utils::index_vector(u, 2), -1);
}

TEST(check_in, check_in) {
    std::vector<int> a = { 9, 8, 7 };
    std::list<int> b = { 9, 9, 1, -2, 0 };
    EXPECT_TRUE(trc::utils::check_in(8, a));
    EXPECT_FALSE(trc::utils::check_in(10, a));
    EXPECT_TRUE(trc::utils::check_in(-2, b));
    EXPECT_TRUE(trc::utils::check_in(0, b));
    EXPECT_FALSE(trc::utils::check_in(-1, b));
}

TEST(check_in, check_in_i) {
    std::list<std::string> b
        = { "apple", "bananas", "pear" };
    std::list<int> c = { 9, 9, 1, -2, 0 };
    EXPECT_EQ(trc::utils::check_in_i("apple", b), 0);
    EXPECT_EQ(trc::utils::check_in_i("watermelon", b), -1);
    EXPECT_EQ(trc::utils::check_in_i(1, c), 2);
    EXPECT_EQ(trc::utils::check_in_i(-10, c), -1);
}

TEST(check_in, s_check_in_s) {
    std::vector<int> a = { 9, 8, 7 };
    std::deque<int> i = { 10 };
    std::list<std::string> b
        = { "apple", "bananas", "pear" };
    std::vector<std::string> c = { "apple", "pear" };

    EXPECT_TRUE(trc::utils::s_check_in_s(c, b));
    EXPECT_FALSE(trc::utils::s_check_in_s(i, a));
}

TEST(data, map_check_in) {
    std::map<int, std::string> a
        = { { 0, "pp" }, { 10, "op" }, { 90, "ui" } };
    EXPECT_TRUE(trc::utils::map_check_in_first(a, 0));
    EXPECT_FALSE(trc::utils::map_check_in_first(a, 2));
    EXPECT_TRUE(trc::utils::map_check_in_second(a, "pp"));
}

TEST(data, sizeof_static_array) {
    int a[10];
    EXPECT_EQ(trc::utils::sizeof_static_array(a), 10);

    char t[19];
    EXPECT_EQ(trc::utils::sizeof_static_array(t), 19);
}
