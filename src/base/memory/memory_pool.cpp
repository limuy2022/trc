/**
 * 内存池实现
 * 有点像stl中的内存分配模式
 * 以空间换时间
 */

#include <base/Error.h>
#include <base/memory/memory_pool.h>
#include <base/trcdef.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <language/error.h>
#include <utility>

// 初次申请的node_mem个数
#define INIT_NODE_SIZE 500
// 再次申请的node_mem个数
#define REALLOC_SIZE 50
// full_gc启动条件
#define condit_for_full_gc 500

/**
 * @brief 获取与需要申请的大小相适配的内存块链表
 * @param size_ 需要申请的实际内存大小
 * @return int 对应的链表编号
 */
inline static int get_list(size_t size_) {
    return int(std::ceil(size_ * 1.0 / MEMORY_BLOCKS_SIZE) - 1);
}

/**
 * @brief
 * 获取与链表头索引(memory_heads)相配的内存块大小
 */
inline static int get_block_size(int index) {
    return (index + 1) * MEMORY_BLOCKS_SIZE;
}

namespace trc::memory {
namespace memclass {
    /**
     * @brief 管理内存块的链表节点
     * 由于为用户预先分配的空间并未使用，所以用来储存下一个节点的指针
     * 下一个节点：xxx->next
     * 为用户准备的首地址：(void*)(xxx->next)
     */
    union node_mem {
        node_mem* next = nullptr;

        explicit node_mem(def::byte_t* addr);
    };

    node_mem::node_mem(def::byte_t* addr)
        : next((node_mem*)addr) {
    }
}

using namespace memclass;

void memory_pool::delete_list(int num) {
    auto ptr = memory_heads[num];
    if (!ptr) {
        return;
    }
    node_mem *now = ptr->next, *tmp;
    while (now) {
        tmp = now->next;
        delete now;
        now = tmp;
    }
}

void memory_pool::init_list(int num) {
    // 先还原头结点
    size_t blocks_of_size = get_block_size(num);
    auto* mem_tmp = (def::byte_t*)malloc(INIT_NODE_SIZE * blocks_of_size);
    malloc_mem_heads.push_back(mem_tmp);
    if (!mem_tmp) {
        error::send_error(error::MemoryError, language::error::memoryerror);
    }
    node_mem*& head = memory_heads[num];
    head = new node_mem(mem_tmp);
    node_mem* now = head;
    auto* now_begin_of_mem = mem_tmp + blocks_of_size;
    for (int i = 0;; ++i, now_begin_of_mem += blocks_of_size) {
        now->next = new node_mem(now_begin_of_mem);
        if (i == INIT_NODE_SIZE) {
            now->next->next = nullptr;
            break;
        }
        now = now->next;
    }
}

void memory_pool::malloc_more(int num) {
    node_mem*& head = memory_heads[num];
    int block_size = get_block_size(num);
    // 这十五个另外开辟内存空间，避免影响到原有的内存
    auto* mem_tmp = (def::byte_t*)malloc(REALLOC_SIZE * block_size);
    malloc_mem_heads.push_back(mem_tmp);
    if (!mem_tmp)
        error::send_error(error::MemoryError, language::error::memoryerror);
    auto* now_begin_of_mem = mem_tmp;
    auto now = head;
    for (int i = 0;; ++i, now_begin_of_mem += block_size) {
        now->next = new node_mem(now_begin_of_mem);
        if (i == REALLOC_SIZE) {
            now->next->next = nullptr;
            break;
        }
        now = now->next;
    }
}

void* memory_pool::mem_realloc(void* p, size_t before, size_t size_) {
    if (before > MEMORY_CUT_LINE && size_ > MEMORY_CUT_LINE) {
        // 都超过memory_pool分割线，意味着全都是利用malloc和realloc分配
        void* tmp = realloc(p, size_);
        if (!tmp)
            error::send_error(error::MemoryError, language::error::memoryerror);
        return tmp;
    }
    if (get_list(before) == get_list(size_)) {
        // 两者都处在同一个内存节点的最大值内，原样返回
        return p;
    }
    void* res = this->mem_malloc(size_);
    memcpy(res, p, before);
    this->mem_free(p, before);
    return res;
}

/**
 * @brief
 * 从node_mem中获取实际内存地址并重新连接链表
 */
inline static void* get_node_address(node_mem* fl_head) {
    void* re = fl_head->next;
    fl_head->next = fl_head->next->next;
    return re;
}

void* memory_pool::mem_malloc(size_t size_) {
    if (size_ > MEMORY_CUT_LINE) {
        return malloc(size_);
    }
    // 找一块合适的大小，从自由链表中取出，自动补齐到MEMORY_BLOCKS_SIZE的倍数

    // 向上取整，找到合适的链表
    int index = get_list(size_);
    auto& fl_head = memory_heads[index];
    // 内存不足有两种情况，一种是分配过，但是用完了，另一种是未使用过
    if (!fl_head) {
        // 未使用
        init_list(index);
        return get_node_address(fl_head);
    }
    if (fl_head->next == nullptr) {
        // 用完了内存，向操作系统申请内存扩容
        malloc_more(index);
        ++malloc_from_os;
        return get_node_address(fl_head);
    }
    //内存充足
    if (malloc_from_os >= condit_for_full_gc) {
        full_gc();
    }
    return get_node_address(fl_head);
}

void memory_pool::mem_free(void* p, size_t size_) {
    if (size_ > MEMORY_CUT_LINE) {
        free(p);
        return;
    }
    int list_index = get_list(size_);
    auto& fl_head = memory_heads[list_index];
    auto insert = (node_mem*)(p);
    insert->next = fl_head->next;
    fl_head->next = insert;
}

memory_pool::~memory_pool() {
    for (int i = 0; i < MEMORY_LISTS_NUM; ++i) {
        // 释放内存链表
        delete_list(i);
    }
    // 释放内存块
    for (auto i : malloc_mem_heads) {
        free(i);
    }
}

void memory_pool::full_gc() {
}
}
