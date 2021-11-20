/**
 * 内存池，所有虚拟机共享一个
 * 
 * 原理是首先分配大块内存，然后进行分区
 * 申请内存就是把内存地址返回，释放内存就是标记为空闲
 */

#ifndef TRC_INCLUDE_MEMORY_MEMORY_POOL_H
#define TRC_INCLUDE_MEMORY_MEMORY_POOL_H

#include <cstddef>

using namespace std;

namespace memclass {
    union node_mem;
}

class memory_pool {
    /**
     * 内存池
     * 注：对象存放在对象池中，由对象创建的特殊对象存放在公共对象池中
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

public:
    memory_pool();

    ~memory_pool();

    // 效果同malloc，free，realloc
    void *mem_malloc(size_t size_);

    void mem_free(void *p, size_t size_);

    void *mem_realloc(void *p, size_t before, size_t size_);

private:
    void full_gc();

    // 储存各个链表的链表头
    memclass::node_mem ** memory_heads;
    // 储存每个链表的node_mem个数
    int *size_of_node;

    void malloc_more(int);

    void delete_list(int num);

    void init_list(int num);

    // 向外申请内存的次数，达到一定程度就启动full_gc
    int malloc_from_os = 0;
};

#endif
