/**
 * 内存池实现
 * 有点像stl中的内存分配模式
 * 以空间换时间
 */

#include <cstdlib>
#include <utility>
#include <cmath>
#include "trcdef.h"
#include "memory/memory_pool.h"
#include "Error.h"

// 内存池一二级分配器分界线
#define MEMORY_CUT_LINE 128
// 每个内存链表之间的差值
#define MEMORY_BLOCKS_SIZE 8
// 一个内存链表的初始大小
#define MEMORY_BASE_SIZE 128
// 内存链表个数
#define MEMORY_LISTS_NUM MEMORY_BASE_SIZE / MEMORY_BLOCKS_SIZE

inline static int get_list(int size_) {
    /**
     * 获取与大小相适配的内存块链表
     */
    return ceil(size_ * 1.0 / MEMORY_BLOCKS_SIZE) - 1;
}
inline static int get_block_size(int index) {
    /**
     * 获取与链表头索引(memory_heads)相配的内存块大小
     */
    return (index + 1) * MEMORY_BLOCKS_SIZE;
}

using namespace std;

#define get_next(u) ((*((u)->address)))

namespace memclass {
    union node_mem {
        /**
         * 节点
         * 由于为用户预先分配的空间闲着也是闲着，所以用来储存下一个节点的指针
         * 下一个节点：get_next(next)
         * 为用户准备的首地址：(void*)(xxx->address)
         */
        node_mem ** address = nullptr;
        explicit node_mem(byte_t *addr);
    };

    node_mem::node_mem(byte_t *addr) :
            address((node_mem**)addr) {}
}

using namespace memclass;

void memory_pool::delete_list(int num) {
    /**
     * 释放整个链表的内存
     */
     auto tmp_h = memory_heads[num];
     if(!tmp_h) {
        return;
    }
    node_mem *now = get_next(tmp_h), *tmp;

    for(int i = 0; i < size_of_node[num]; ++i) {
        tmp = get_next(now);
        delete now;
        now = tmp;
    }
}

// 初次申请的node_mem个数
#define INIT_NODE_SIZE 10

void memory_pool::init_list(int num) {
    /**
     * 初始化head，为它分配内存
     * num:list编号，作用是推测出node_mem的大小
     * 例如list1，大小为8
     */
    // 先还原头结点
    size_t blocks_of_size = get_block_size(num);
    auto *mem_tmp = (byte_t*)malloc(INIT_NODE_SIZE * blocks_of_size);
    if(!mem_tmp) {
        send_error(MemoryError, "can't get the memory from os.");
    }
    node_mem * &head = memory_heads[num];
    head = new node_mem(mem_tmp);
    node_mem* now = head;
    auto *now_begin_of_mem = mem_tmp + blocks_of_size;
    for (int i = 0; i < INIT_NODE_SIZE; ++i, now_begin_of_mem+=blocks_of_size) {
        get_next(now) = new node_mem(now_begin_of_mem);
        now = get_next(now);
    }
    get_next(now) = nullptr;
    // 储存数据个数
    size_of_node[num] = INIT_NODE_SIZE;
}

#undef INIT_NODE_SIZE

// 再次申请的node_mem个数
#define REALLOC_SIZE 15

void memory_pool::malloc_more(int num) {
    /**
     * 申请更多内存，
     */
    node_mem* &head = memory_heads[num];
    int block_size = get_block_size(num);
    // 这十五个另外开辟内存空间，避免影响到原有的内存
    auto * mem_tmp = (byte_t*)malloc(REALLOC_SIZE * block_size);
    if(!mem_tmp) {
        send_error(MemoryError, "can't get the memory from os.");
    }
    auto *now_begin_of_mem = mem_tmp;
    auto now = head;
    for (int i = 0; i < REALLOC_SIZE; ++i, now_begin_of_mem+=block_size) {
        get_next(now) = new node_mem(now_begin_of_mem);
        now = get_next(now);
    }
    get_next(now) = nullptr;
    size_of_node[num] += REALLOC_SIZE;
}

void *memory_pool::mem_realloc(void *p, size_t before, size_t size_) {
    /**
     * 重新设置大小
     * p:原先的地址
     * before:之前的大小
     * size_:需要申请的大小
     */
    if (before > MEMORY_CUT_LINE && size_ > MEMORY_CUT_LINE) {
        void * tmp = realloc(p, size_);
        if(!tmp) {
            send_error(MemoryError, "can't get the memory from os.");
        }
        return tmp;
    }
    if(get_list(before) == get_list(size_)) {
        return p;
    }
    this->mem_free(p, before);
    return this->mem_malloc(size_);
}

inline static void *get_node_address(node_mem * fl_head, int &num) {
    /**
     * 从node_mem中获取实际内存地址并重新连接链表
     */

    void *re = get_next(fl_head);
    get_next(fl_head) = get_next(get_next(fl_head));
    num--;
    return re;
}

//full_gc启动条件
#define condit_for_full_gc 500

void *memory_pool::mem_malloc(size_t size_) {
    /**
     * 申请内存
     * 注意：申请失败后会报出MemoryError
     */
    if (size_ > MEMORY_CUT_LINE) {
        return malloc(size_);
    }
    // 找一块合适的大小，从自由链表中取出，自动补齐到MEMORY_BLOCKS_SIZE的倍数

    // 向上取整，找到合适的链表
    int index = get_list(size_);
    auto &fl_head = memory_heads[index];
    // 内存不足有两种情况，一种是分配过，但是用完了，另一种是未使用过
    if (!fl_head) {
        // 未使用
        init_list(index);
        return get_node_address(fl_head, size_of_node[index]);
    }
    if (size_of_node[index] == 0) {
        // 用完了内存，向操作系统申请内存扩容
        malloc_more(index);
        ++malloc_from_os;
        return get_node_address(fl_head, size_of_node[index]);
    }
    //内存充足
    if(malloc_from_os >= condit_for_full_gc) {
        full_gc();
    }
    return get_node_address(fl_head, size_of_node[index]);
}

#undef condit_for_full_gc
#undef REALLOC_SIZE

void memory_pool::mem_free(void *p, size_t size_) {
    /**
     * 释放内存
     */
    if (size_ > MEMORY_CUT_LINE) {
        free(p);
    }
    int index = get_list(size_);
    auto &fl_head = memory_heads[index];
    auto insert = (node_mem *) (p);
    get_next(insert) = get_next(fl_head);
    get_next(fl_head) = insert;
}

memory_pool::memory_pool() {
    // 分配内存链表头
    memory_heads = new node_mem *[MEMORY_LISTS_NUM]();
    size_of_node = new int[MEMORY_LISTS_NUM]();
}

memory_pool::~memory_pool() {
    for (int i = 0; i < MEMORY_LISTS_NUM; ++i) {
        delete_list(i);
    }
    delete[] memory_heads;
    delete[] size_of_node;
}

void memory_pool::full_gc() {
    /**
     * 由于该内存池按照既有模式只申请不释放，所以设置该内存回收装置，当向外申请内存超过500遍时执行进行自我回收
     */
}
