/**
 * 内存池，所有虚拟机共享一个
 *
 * 原理是首先分配大块内存，然后进行分区
 * 申请内存就是把内存地址返回，释放内存就是标记为空闲
 */

#pragma once

// 内存池一二级分配器分界线
#define MEMORY_CUT_LINE 128
// 每个内存链表之间的差值
#define MEMORY_BLOCKS_SIZE 8
// 一个内存链表的初始大小
#define MEMORY_BASE_SIZE 128
// 内存链表个数
#define MEMORY_LISTS_NUM MEMORY_BASE_SIZE / MEMORY_BLOCKS_SIZE

#include <base/library.h>
#include <vector>

namespace trc::memory {
namespace memclass {
    union node_mem;
}

/**
 * @brief 内存池
 * 对象存放在对象池中，由对象创建的特殊对象存放在公共对象池中
 * 只有对象申请的内存才会被存放到内存池中
 * 两者的区别在于：对象池大小固定，效率更高；
 * 而内存池可以申请大小不固定的内存，效率稍低
 * **********************************************
 * 必看事项：
 * 1.本内存池底层由malloc分配内存，所以不可以支持任何跟继承扯上关系的类和结构体
 * （但可以分配非基础数据类型，建议仅仅用于分配基础数据类型或者C语言风格的结构体）
 * **********************************************
 * 实现方式：
 * 小于等于128k的用第二级分配器
 * 大于128k的用第一级分配器
 *
 * 有16个free-lists。各自管理大小不同的小额区块。
 * 将不论什么小额区块的内存需求量上调至8的倍数。如需求30，则上调至32。
 */
class TRC_base_api memory_pool {
public:
    /**
     * @brief
     * 析构时假设所有已申请的内存均已返还给内存池
     */
    ~memory_pool();

    /**
     * @brief 申请内存，对标malloc
     * @warning 申请失败后会报出MemoryError
     */
    void* mem_malloc(size_t size_);

    /**
     * @brief 释放内存，对标free
     */
    void mem_free(void* p, size_t size_);

    /**
     * @brief 重新设置大小，对标realloc
     * @param p 原先的地址
     * @param before 之前的大小
     * @param size_ 需要申请的大小
     */
    void* mem_realloc(void* p, size_t before, size_t size_);

private:
    /**
     * @brief 深度回收内存
     * 由于该内存池按照既有模式只申请不释放，所以设置该内存回收装置，当向外申请内存超过500遍时执行进行自我回收
     */
    void full_gc();

    // 储存各个链表的链表头
    memclass::node_mem* memory_heads[MEMORY_LISTS_NUM] { nullptr };
    // 储存所有申请的内存块
    std::vector<void*> malloc_mem_heads;

    /**
     * @brief 申请更多内存，
     */
    void malloc_more(int);

    /**
     * @brief 释放整个链表的内存
     */
    void delete_list(int num);

    /**
     * @brief 初始化head，为它分配内存
     * @param num
     * list编号，作用是推测出node_mem的大小，例如list为1，大小为8
     */
    void init_list(int num);

    // 向外申请内存的次数，达到一定程度就启动full_gc
    int malloc_from_os = 0;
};
}
