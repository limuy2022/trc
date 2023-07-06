#include <deque>
#include <gtest/gtest.h>
#include <list>
#include <map>
#include <vector>
import trcdef;
import data;
import filesys;

using namespace trc;

TEST(data, inrange) {
    EXPECT_FALSE(utils::inrange(6, 1, 9));
    EXPECT_TRUE(utils::inrange(0, 9, 5));
    EXPECT_TRUE(utils::inrange(-5, -1, -2));
    EXPECT_FALSE(utils::inrange(-9, 0, 1));
}

TEST(data, index_vector) {
    std::list<int> i = { 9, 11, 2, 11, 11 };
    EXPECT_EQ(trc::utils::index_vector(i, 11), 1);

    std::vector<int> u = { 8, 8, 9 };
    EXPECT_EQ(trc::utils::index_vector(u, 8), 0);
    EXPECT_EQ(trc::utils::index_vector(u, 2), -1);
}

TEST(data, index_strstl) {
    std::list<const char*> p = { "ppp", "lll", "pp" };
    EXPECT_EQ(utils::index_vector(p, "ppp"), 0);
    EXPECT_EQ(utils::index_vector(p, "pp"), 2);
    EXPECT_EQ(utils::index_vector(p, "llll"), -1);
}

TEST(check_in, str_check_in) {
    std::vector<const char*> a = {};
    EXPECT_FALSE(utils::str_check_in("pp", a.begin(), a.end()));
    std::vector<const char*> b = { "ppp", "ooo", "llll" };
    EXPECT_FALSE(utils::str_check_in("lll", b.begin(), b.end()));
    EXPECT_TRUE(utils::str_check_in("ooo", b.begin(), b.end()));
}

TEST(check_in, check_in_i) {
    std::list<std::string> b = { "apple", "bananas", "pear" };
    std::list<int> c = { 9, 9, 1, -2, 0 };
    EXPECT_EQ(trc::utils::check_in_i("apple", b.begin(), b.end()), 0);
    EXPECT_EQ(trc::utils::check_in_i("watermelon", b.begin(), b.end()), -1);
    EXPECT_EQ(trc::utils::check_in_i(1, c.begin(), c.end()), 2);
    EXPECT_EQ(trc::utils::check_in_i(-10, c.begin(), c.end()), -1);
}

TEST(check_in, str_check_in_i) {
    std::vector<const char*> a = { "qbchj", "", "qbch" };
    EXPECT_EQ(utils::str_check_in_i("", a.begin(), a.end()), 1);
    EXPECT_EQ(utils::str_check_in_i(" ", a.begin(), a.end()), -1);
    EXPECT_EQ(utils::str_check_in_i("qbch", a.begin(), a.end()), 2);
    EXPECT_EQ(utils::str_check_in_i("qbchj", a.begin(), a.end()), 0);
}

TEST(data, map_check_in) {
    std::map<int, std::string> a = { { 0, "pp" }, { 10, "op" }, { 90, "ui" } };
    EXPECT_TRUE(trc::utils::map_check_in_first(a, 0));
    EXPECT_FALSE(trc::utils::map_check_in_first(a, 2));
    EXPECT_TRUE(trc::utils::map_check_in_second(a, "pp"));
}

// 求静态数组长度
TEST(data, sizeof_static_array) {
    int a[10];
    EXPECT_EQ(trc::utils::sizeof_static_array(a), 10);

    char t[19];
    EXPECT_EQ(trc::utils::sizeof_static_array(t), 19);
}

// 测试求整型长度函数
TEST(data, len) {
    EXPECT_EQ(trc::utils::len(0), 1);
    EXPECT_EQ(trc::utils::len(900000), 6);
    EXPECT_EQ(trc::utils::len(10), 2);
    EXPECT_EQ(trc::utils::len(1000000000), 10);
    EXPECT_EQ(trc::utils::len(-1), 1);
    EXPECT_EQ(trc::utils::len(-1000000), 7);
    EXPECT_EQ(trc::utils::len(-1102030), 7);
    EXPECT_EQ(trc::utils::len((unsigned int)0), 1);
    EXPECT_EQ(trc::utils::len((unsigned int)100), 3);
}

// 测试判断浮点数相等的函数
TEST(data, isequal) {
    EXPECT_TRUE(utils::isequal(1.0, 1.0));
    EXPECT_FALSE(utils::isequal(1.0, 1.1));
    EXPECT_FALSE(utils::isequal(1.0, 2.0));
}
