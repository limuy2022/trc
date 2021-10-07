/**
 * 内存池，所有虚拟机共享一个
 * 
 * 原理是首先分配大块内存，然后进行分区
 * 申请内存就是把内存地址返回，释放内存就是标记为空闲
 */ 

#ifndef TRC_INCLUDE_MEMORY_MEMORY_POOL_H
#define TRC_INCLUDE_MEMORY_MEMORY_POOL_H

// 内存池初始大小，实际内存大小，以字节为单位
#define MEMORY_INIT_SIZE 1024

#include <cstdlib>

namespace memory_pool_class{
    class node;
    class freespace;
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
     * 本内存池底层由malloc分配内存，所以不可以支持任何跟继承扯上关系的类和结构体
     * （但可以分配非基础数据类型，建议仅仅用于分配基础数据类型）
     */

public:
    memory_pool(size_t size_ = MEMORY_INIT_SIZE);

    // 效果同malloc，free，realloc
    void *mem_malloc(size_t size_);

    void mem_free(void *p);

    void* mem_realoc(void *p, size_t size_);

private:
    char *mem_base;
};

#endif
