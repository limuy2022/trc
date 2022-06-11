#include <gtest/gtest.h>
#include <base/utils/type.hpp>

using namespace trc;

// 测试类型转换
TEST(type, totype) {
    EXPECT_EQ(utils::to_type<int>(1.9), 1);
    EXPECT_EQ(utils::to_type<int>("90"), 90);
    EXPECT_EQ(utils::to_type<double>(90), 90.0);
}
