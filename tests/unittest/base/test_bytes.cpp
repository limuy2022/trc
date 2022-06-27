#include <base/utils/bytes.h>
#include <gtest/gtest.h>
#include <vector>

using namespace trc;

static void check_bytes(const std::vector<def::byte_t>& expect_data,
    const std::vector<def::byte_t>& actual) {
    ASSERT_EQ(expect_data.size(), actual.size());
    for (size_t i = 0, n = expect_data.size(); i < n; ++i) {
        EXPECT_EQ(expect_data[i], actual[i]);
    }
}

static void test_bytes_order_change(std::vector<def::byte_t> origin_data,
    const std::vector<def::byte_t>& expect) {
    utils::bytes_order_change(&origin_data[0], origin_data.size());
    check_bytes(expect, origin_data);
}

// 测试改变大小端函数
TEST(bytes, bytes_order_change) {
    test_bytes_order_change({ 0x32, 0x64 }, { 0x64, 0x32 });
    test_bytes_order_change({ 0x12, 0x12, 0x12 }, { 0x12, 0x12, 0x12 });
    test_bytes_order_change({ 0x34, 0x35, 0x36 }, { 0x36, 0x35, 0x34 });
    test_bytes_order_change({ 0x0 }, { 0x0 });
    test_bytes_order_change({}, {});
}
