/**
 * 对象池，每个TVM一个，属于gc_pool的一部分
 * 
 * 原理是首先分配大块内存，然后进行分区
 * 申请内存就是把内存地址返回，释放内存就是标记为空闲
 */ 

#ifndef TRC_INCLUDE_MEMORY_OBJS_POOL_HPP
#define TRC_INCLUDE_MEMORY_OBJS_POOL_HPP

#include <vector>
#include "mem.h"

using namespace std;

// 对象池初始大小(对象个数，并不是内存大小)，可以在此基础上扩容
#define OBJS_POOL_SIZE 1000

namespace objs_pool_objs {
    class node_ {
    public:
        node_* next = nullptr;
        void* data;
    };
}

using namespace objs_pool_objs;

template<typename T>
class objs_pool
{
    /**
     * 对象池，每次默认分配OBJS_POOL_SIZE个对象
     * 使用双向链表进行管理
     * 另外，对于该内存池，注重的是分配速度，而不是释放速度，释放会由gc统一管理
     */
public:
    // 已申请对象个数
    size_t alloc_objs;

    objs_pool(gc_obj *con, size_t obj_num = OBJS_POOL_SIZE);

    ~objs_pool();

    T *trcmalloc();

    // 这个链表需要被遍历判断是否为垃圾对象，所以设成public
    node_ *used_head;

    node_ *free_head;
private:
    vector<T*> arr;

    // 对于gc管理者的链接，可以反向调用gc
    gc_obj* con;

    // 此函数为真正实现内存分配的函数
    // 而提供外调的接口作用为判断内存是否充足并且调用gc进行垃圾回收
    T* malloc_private();
};

template<typename T>
objs_pool<T>::objs_pool(gc_obj *con, size_t obj_num):
    con(con),
    alloc_objs(obj_num),
    arr(obj_num),
    used_head(new node_),
    free_head(new node_)
{
    node_ *now = free_head, *t;
    for (int i = 0; i < obj_num; ++i) {
        t = new node_;
        arr[i] = new T;
        t -> data = arr[i];
        now -> next = t;
        now = t;
    }
}

#define FREE_LIST(str) \
do{\
    node_ *now = (str), *n;\
    while (now != nullptr) {\
        n = now -> next;\
        delete now;\
        now = n;\
    }\
} while(0)

template<typename T>
objs_pool<T>::~objs_pool() {
    FREE_LIST(free_head);
    FREE_LIST(used_head);
    for(auto &i: arr)
        delete i;
}

#undef FREE_LIST

template<typename T>
T* objs_pool<T>::malloc_private() {
    /**
     * 私有的将实际操作分离
     */ 
    
    // 保存需要返回的节点
    node_ *now_use = free_head -> next;
    // 使头结点的下一个节点指向当前节点的下一个节点
    free_head -> next = now_use -> next;
    // 使当前节点的下一个节点指向原本头结点的下一个节点
    now_use -> next = used_head -> next;
    // 使头结点的下一个节点指向当前节点
    used_head -> next = now_use;

    return (T*)now_use -> data;
}

template<typename T>
T* objs_pool<T>::trcmalloc() {
    if (free_head->next != nullptr) {
        // 内存充足，可以直接使用
        return malloc_private();
    }
    // 内存不足，启动gc
    con -> gc();
    if(free_head -> next == nullptr) {
        // 启动gc后内存仍然不足，向操作系统再次申请内存
        ++alloc_objs;
        T* tmp = new T;
        arr.push_back(tmp);
        node_ *new_n = new node_;
        new_n -> data = tmp;
        // 直接将节点连接到使用链表中
        new_n -> next = used_head -> next;
        used_head -> next = new_n;
        return (T*)new_n -> data;
    }
    return malloc_private();
}

#undef OBJS_POOL_SIZE

#endif
