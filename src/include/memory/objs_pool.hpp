/**
 * 对象池，所有TVM公用，属于gc_pool的一部分
 * 
 * 原理是首先分配大块内存，然后进行分区
 * 申请内存就是把内存地址返回，释放内存就是标记为空闲
 */ 

#ifndef TRC_INCLUDE_MEMORY_OBJS_POOL_HPP
#define TRC_INCLUDE_MEMORY_OBJS_POOL_HPP

#include <list>
#include <vector>
#include "mem.h"

using namespace std;

// 对象池初始大小(对象个数，并不是内存大小)，可以在此基础上扩容
#define OBJS_POOL_SIZE 1000

template<typename T>
class objs_pool
{
    /**
     * 对象池，每次默认分配OBJS_POOL_SIZE个对象
     * 使用双向链表进行管理
     * 另外，对于该内存池，注重的是分配速度，而不是释放速度，释放会由gc统一管理
     */
public:
    objs_pool(size_t obj_num = OBJS_POOL_SIZE);

    template<typename ... P>
    T *trcmalloc(const P& ... data_argv);
    
private:
    vector<T> arr;

    // 此函数为真正实现内存分配的函数
    // 而提供外调的接口作用为判断内存是否充足并且调用gc进行垃圾回收
    T* malloc_private();

    void gc();

    list<T*> used_head, free_head;
};

template<typename T>
objs_pool<T>::objs_pool(size_t obj_num):
    arr(obj_num),
    free_head(obj_num)
{
    typename list<T*>::iterator tmp = free_head.begin();
    for (int i = 0; i < obj_num; ++i) {
        *tmp = &arr[i];
        ++tmp;
    }
}

template<typename T>
T* objs_pool<T>::malloc_private() {
    /**
     * 私有的将实际操作分离
     */

    T* tmp = free_head.front();
    free_head.pop_front();
    used_head.push_front(tmp);
    return tmp;
}

template<typename T>
template<typename ... P>
T * objs_pool<T>::trcmalloc(const P& ... data_argv)  {
    if (!free_head.empty()) {
        // 内存充足，可以直接使用
        return new(malloc_private()) T(data_argv...);
    }
    // 内存不足，启动gc
    this -> gc();
    if(free_head.empty()) {
        // 启动gc后内存仍然不足，向操作系统再次申请内存
        size_t index = arr.size();
        arr.resize(index + 1);
        T * tmp = &arr[index];
        used_head.push_front(tmp);
        return new(tmp) T(data_argv...);
    }
    return new(malloc_private()) T(data_argv...);
}

template<typename T>
void objs_pool<T>::gc() {
    /**
     * 垃圾回收
     * 垃圾回收算法采用引用计数回收
     * 至于相互引用的漏洞，将会采用可达性回收算法进行
     *
     * 注意，在垃圾回收中，所有虚拟机中的对象统一继承于trcobj，以便在TVM析构时进行垃圾回收
     *
     * 回收条件：
     * 1.当单个对象池处于满状态时
     * 
     * 当垃圾回收运行时，字节码运行也暂停，这里并不会采取多线程运行，也不允许出现内存访问错误的现象
     */
    T* tmp;
    typename list<T*>::iterator index_re = used_head.begin();
    for(int i = 0, n = used_head.size(); i < n; ++i) {
        tmp = *index_re;
        if(!tmp -> refs) {
            tmp -> delete_();
            typename list<T*>::iterator tmp_iter = index_re++;
            used_head.erase(tmp_iter);
            free_head.push_front(tmp);
        } else{
            ++index_re;
        }
    }
}

#undef OBJS_POOL_SIZE

#endif
