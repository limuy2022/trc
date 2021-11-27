/**
 * 内存池实现
 * 有点像stl中的内存分配模式
 * 以空间换时间
 */

#include <cstdlib>
#include <utility>
#include <cmath>
#include "base/trcdef.h"
#include "base/memory/memory_pool.h"
#include "base/Error.h"
#include "base/dll.h"

inline static int get_list(size_t size_) {
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

namespace trc {
    namespace memory {
        namespace memclass {
            union node_mem {
                /**
                 * 节点
                 * 由于为用户预先分配的空间闲着也是闲着，所以用来储存下一个节点的指针
                 * 下一个节点：xxx->next
                 * 为用户准备的首地址：(void*)(xxx->next)
                 */
                node_mem* next = nullptr;

                explicit node_mem(def::byte_t* addr);
            };

            node_mem::node_mem(def::byte_t* addr) :
                next((node_mem*)addr) {}
        }

        using namespace memclass;

        void memory_pool::delete_list(int num) {
            /**
             * 释放整个链表的内存
             */
            auto ptr = memory_heads[num];
            if(!ptr) {
                return;
            }
            node_mem *now = ptr -> next, *tmp;
            while (now) {
                tmp = now -> next;
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
            auto *mem_tmp = (def::byte_t *) malloc(INIT_NODE_SIZE * blocks_of_size);
            malloc_mem_heads.push_back(mem_tmp);
            if (!mem_tmp) {
                error::send_error(error::MemoryError, "can't get the memory from os.");
            }
            node_mem *&head = memory_heads[num];
            head = new node_mem(mem_tmp);
            node_mem * now = head;
            auto *now_begin_of_mem = mem_tmp + blocks_of_size;
            for (int i = 0; ; ++i, now_begin_of_mem += blocks_of_size) {
                now -> next = new node_mem(now_begin_of_mem);
                if(i == INIT_NODE_SIZE) {
                    now -> next -> next = nullptr;
                    break;
                }
                now = now-> next;
            }
        }
        
        #undef INIT_NODE_SIZE
        
        // 再次申请的node_mem个数
        #define REALLOC_SIZE 15
        
        void memory_pool::malloc_more(int num) {
            /**
             * 申请更多内存，
             */
            node_mem *&head = memory_heads[num];
            int block_size = get_block_size(num);
            // 这十五个另外开辟内存空间，避免影响到原有的内存
            auto *mem_tmp = (def::byte_t *) malloc(REALLOC_SIZE * block_size);
            malloc_mem_heads.push_back(mem_tmp);
            if (!mem_tmp)
                error::send_error(error::MemoryError, "can't get the memory from os.");
            auto *now_begin_of_mem = mem_tmp;
            auto now = head;
            for (int i = 0; ; ++i, now_begin_of_mem += block_size) {
                now -> next = new node_mem(now_begin_of_mem);
                if(i == REALLOC_SIZE) {
                    now -> next -> next = nullptr;
                    break;
                }
                now = now-> next;
            }
        }
        
        void *memory_pool::mem_realloc(void *p, size_t before, size_t size_) {
            /**
             * 重新设置大小
             * p:原先的地址
             * before:之前的大小
             * size_:需要申请的大小
             */
        
            if (before > MEMORY_CUT_LINE && size_ > MEMORY_CUT_LINE) {
                void *tmp = realloc(p, size_);
                if (!tmp)
                    error::send_error(error::MemoryError, "can't get the memory from os.");
                return tmp;
            }
            if (get_list(before) == get_list(size_)) {
                return p;
            }
            this->mem_free(p, before);
            return this->mem_malloc(size_);
        }
        
        inline static void *get_node_address(node_mem *fl_head) {
            /**
             * 从node_mem中获取实际内存地址并重新连接链表
             */
        
            void *re = fl_head -> next;
            fl_head -> next = fl_head -> next -> next;
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
                return get_node_address(fl_head);
            }
            if (fl_head->next==nullptr) {
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
        
        #undef condit_for_full_gc
        #undef REALLOC_SIZE
        
        void memory_pool::mem_free(void *p, size_t size_) {
            /**
             * 释放内存
             */
            if (size_ > MEMORY_CUT_LINE) {
                free(p);
                return;
            }
            int list_index = get_list(size_);
            auto &fl_head = memory_heads[list_index];
            auto insert = (node_mem *) (p);
            insert -> next = fl_head -> next;
            fl_head -> next = insert;
        }
        
        memory_pool::~memory_pool() {
            /**
             * 析构时假设所有内存均已返还给内存池
             */
            for (int i = 0; i < MEMORY_LISTS_NUM; ++i) {
                // 释放内存链表
                delete_list(i);
            }
            // 释放内存块
            for(auto i : malloc_mem_heads) {
                free(i);
            }
        }
        
        void memory_pool::full_gc() {
            /**
             * 由于该内存池按照既有模式只申请不释放，所以设置该内存回收装置，当向外申请内存超过500遍时执行进行自我回收
             */
        }
    }
}
