#include <base/memory/objs_pool.hpp>
#include <gtest/gtest.h>
#include <test_new_del.hpp>

using namespace trc;

// 对象池构造函数和析构函数测试
TEST(memory, objs_pool_new_delete) {
    count_new_del::reset();
    auto pool = new memory::objs_pool<count_new_del::test_new_del>;
    delete pool;
    EXPECT_NE(count_new_del::del_cnt, 0);
    EXPECT_NE(count_new_del::new_cnt, 0);
    EXPECT_EQ(count_new_del::new_cnt, count_new_del::del_cnt);
}
