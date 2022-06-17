#include <base/memory/memory.h>
#include <base/memory/memory.hpp>
#include <base/memory/objs_pool.hpp>
#include <cstring>
#include <gtest/gtest.h>
#include <list>
#include <stack>
#include <test_new_del.h>
#include <vector>

using namespace trc;

// 内存池测试
TEST(memory, memory_pool) {
    // 申请小的
    int* a = (int*)MALLOC(sizeof(int));
    *a = 10;
    FREE(a, sizeof(int));
    // 申请可变的长字符串
    char* str = (char*)MALLOC(sizeof(char) * 20);
    strcpy(str, "pppp");
    // 往短的方向申请
    str = (char*)REALLOC(str, sizeof(char) * 20, sizeof(char) * 5);
    EXPECT_STREQ(str, "pppp");
    // 往长的方向申请
    str = (char*)REALLOC(str, sizeof(char) * 5, sizeof(char) * 50);
    EXPECT_STREQ(str, "pppp");
    FREE(str, sizeof(char) * 50);
}

// 对象池构造函数和析构函数测试
TEST(memory, objs_pool_new_delete) {
    count_new_del::reset();
    auto pool = new memory::objs_pool<count_new_del::test_new_del>;
    delete pool;
    EXPECT_NE(count_new_del::del_cnt, 0);
    EXPECT_NE(count_new_del::new_cnt, 0);
    EXPECT_EQ(count_new_del::new_cnt, count_new_del::del_cnt);
}

TEST(memory, free_stl) {
    // 检查栈内存是否可以正常释放
    std::stack<int> t1;
    t1.push(1);
    t1.push(2);
    t1.push(3);
    memory::free_stl(t1);
    EXPECT_TRUE(t1.empty());
    // 检查vector内存是否可以正常释放
    std::vector<int> t2;
    // 检查释放空容器
    memory::free_stl(t2);
    EXPECT_TRUE(t2.empty());
    t2 = { 9, 8, 7, 1, 10, 9 };
    memory::free_stl(t2);
    EXPECT_TRUE(t2.empty());
    // 检查list的内存是否可以正常释放
    std::list<int> t3 = { 9, 8, 1, 0 };
    memory::free_stl(t3);
    EXPECT_TRUE(t3.empty());
}

TEST(memory, free_array_array) {
    count_new_del::reset();
    std::vector<count_new_del::test_new_del*> arr(10);
    for (int i = 0; i < 10; i++) {
        arr[i] = new count_new_del::test_new_del[2];
    }
    memory::free_array_array(arr);
    EXPECT_EQ(20, count_new_del::del_cnt);
    EXPECT_EQ(count_new_del::new_cnt, count_new_del::del_cnt);
}

TEST(memory, free_array_obj) {
    count_new_del::reset();
    std::vector<count_new_del::test_new_del*> arr(10);
    for (int i = 0; i < 10; ++i) {
        arr[i] = new count_new_del::test_new_del;
    }
    memory::free_array_obj(arr);
    EXPECT_EQ(10, count_new_del::del_cnt);
    EXPECT_EQ(count_new_del::new_cnt, count_new_del::del_cnt);
}
