/**
 * 内存池实现
 */ 

#include <cstdlib>
#include <utility>
#include "../include/memory/memory_pool.h"
#include "../include/Error.h"

// 内存池一二级分配器分界线
#define MEMORY_CUT_LINE 128 
// 一个内存块的大小
#define MEMORY_BLOCKS_SIZE 8
// 一个内存链表的初始大小
#define MEMORY_BASE_SIZE 128

using namespace std;

// 现在的块的大小，用于保持递增初始化node_mem的length属性
static int size_block_now = 0;

namespace memclass{
    class node_mem {
        /**
         * 节点
         */ 
    public:
        node_mem(int len);

        void *address;
        node_mem * next;
        int length;
    };

    node_mem::node_mem(int len) : length(len)
    {}

    class freespace {
        /**
         * 实际内存空间的分配者，并担任初始化工作
         */ 
    public:
        node_mem head;
        char * membase;

        freespace();
        ~freespace();
    };

    freespace::freespace() :
        membase((char*)(malloc(sizeof(char) * MEMORY_BASE_SIZE))),
        head(size_block_now)
    {
        if(membase == NULL) {
            send_error(MemoryError, "can't get the memory from os.");
        }
        size_block_now += MEMORY_BLOCKS_SIZE;
    }

    freespace::~freespace() {
        free(membase);
    }
}

using namespace memclass;

void *memory_pool::mem_realoc(void *p, size_t size_) {
    /**
     * 重新设置大小
     */ 
    return realloc(p, size_);
}

void *memory_pool::mem_malloc(size_t size_) {
    /**
     * 申请内存
     */ 
    if(size_ > MEMORY_CUT_LINE) {
        return malloc(size_);
    }
    // 找一块合适的大小，从自由链表中取出，自动对齐MEMORY_BLOCKS_SIZE

    // 合适的链表
    int index = size_ / MEMORY_BLOCKS_SIZE;
    if((size_ * 1.0 / MEMORY_BLOCKS_SIZE) != index) {
        ++index;
    }
    auto fl_head = &memory_heads[index].head;
    if(fl_head -> next == nullptr) {
        // 内存空了，执行之前的请求
        free_pri();
        if(fl_head -> next == nullptr) {
            // 仍然为空，向操作系统申请

        }
    }
    
}

void memory_pool::mem_free(void *p, size_t size_) {
    /**
     * 释放内存
     */ 
    if(size_ > MEMORY_CUT_LINE) {
        free(p);
        return;
    }
    // 将释放请求储存
    delete_reqs.push_back(p);
}

memory_pool::memory_pool() {
	size_block_now = 0;
	memory_heads =  new freespace[MEMORY_BASE_SIZE / MEMORY_BLOCKS_SIZE];
    size_block_now = 0;
}

memory_pool::~memory_pool() {
    delete []memory_heads;
}

void memory_pool::free_pri() {
    /**
     * 将之前储存的释放申请一起释放
     * 释放条件：直到释放申请的数量达到一定数量级或者出现申请空间不足再释放
     */ 
    for(auto &i : delete_reqs) {

    }
    delete_reqs.clear();
}
