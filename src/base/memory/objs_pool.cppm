/**
 * 对象池，所有TVM公用
 *
 * 原理是首先分配大块内存，然后进行分区
 * 申请内存就是把内存地址返回，释放内存就是标记为空闲
 *
 * 当对象池使用完毕后，重新开启一块新的内存，且不与之前的内存相连
 */

module;
#include <list>
export module objs_pool;

// 对象池初始大小(对象个数，并不是内存大小)，可以在此基础上扩容
#define OBJS_POOL_SIZE 450
// 对象池每次扩容个数
#define OBJS_ADD_SIZE 15

export namespace trc::memory {
/**
 * 对象池，每次默认分配OBJS_POOL_SIZE个对象
 * 使用双向链表进行管理
 * 另外，对于该内存池，注重的是分配速度，而不是释放速度，释放会由gc统一管理
 */
template <typename T> class objs_pool {
public:
    objs_pool();

    ~objs_pool();

    template <typename... P> T* trcmalloc(const P&... data_argv);

private:
    /**
     * @brief 此函数为真正实现内存分配的函数私有的将实际操作分离
     * @details 而提供外调的接口作用为判断内存是否充足并且调用gc进行垃圾回收
     */
    T* malloc_private();

    /**
     * @brief 生成一块新的内存空间扩大容量
     */
    void add();

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
    void gc();

    /**
     * gc函数只可以改变对象的状态，保证对象持续可用
     * 但该内存池当内存不够时会申请，而full_gc会将
     * 内存始终保持在一个可控的地步，不至于使内存池只进不出，耗尽内存而崩溃
     * 触发条件：gc执行1000次，由gc调用
     */
    void full_gc();

    // 使用后内存
    std::list<T*> used_head;
    // 空闲内存
    std::list<T*> free_head;
    // 所有内存块的首地址
    std::list<T*> blocks;

    // gc运行的次数，执行到一定次数就调用full_gc
    int gc_num = 0;
};

template <typename T>
objs_pool<T>::objs_pool()
    : free_head(OBJS_POOL_SIZE) {
    T* start = new T[OBJS_POOL_SIZE];
    blocks.push_back(start);
    typename std::list<T*>::iterator tmp = free_head.begin();
    for (int i = 0; i < OBJS_POOL_SIZE; ++i, ++tmp) {
        *tmp = start + i;
    }
}

template <typename T> void objs_pool<T>::add() {
    T* add_ptr = new T[OBJS_ADD_SIZE];
    blocks.push_back(add_ptr);
    typename std::list<T*>::iterator add_iter = free_head.end();
    free_head.resize(free_head.size() + OBJS_ADD_SIZE);
    for (int i = 0; i < OBJS_ADD_SIZE; ++i, ++add_iter) {
        *add_iter = add_ptr + i;
    }
}

template <typename T> T* objs_pool<T>::malloc_private() {
    T* tmp = free_head.front();
    free_head.pop_front();
    used_head.push_front(tmp);
    return tmp;
}

template <typename T>
template <typename... P>
T* objs_pool<T>::trcmalloc(const P&... data_argv) {
    if (!free_head.empty()) {
        // 内存充足，可以直接使用
        return new (malloc_private()) T(data_argv...);
    }
    // 内存不足，启动gc
    this->gc();
    if (free_head.empty()) {
        // 启动gc后内存仍然不足，向操作系统再次申请内存
        this->add();
        return new (malloc_private()) T(data_argv...);
    }
    return new (malloc_private()) T(data_argv...);
}

template <typename T> void objs_pool<T>::gc() {
    T* tmp;
    typename std::list<T*>::iterator index_re = used_head.begin();
    for (int i = 0, n = used_head.size(); i < n; ++i) {
        tmp = *index_re;
        if (!tmp->refs) {
            // 显式调用析构函数，然后在原来的内存上重新构建新对象
            tmp->T::~T();
            new (tmp) T();
            typename std::list<T*>::iterator tmp_iter = index_re++;
            used_head.erase(tmp_iter);
            free_head.push_front(tmp);
        } else {
            ++index_re;
        }
    }
    ++gc_num;
    if (++gc_num == 1000) {
        full_gc();
        gc_num = 0;
    }
}

template <typename T> void objs_pool<T>::full_gc() {
}

template <typename T> objs_pool<T>::~objs_pool() {
    for (auto i : blocks) {
        delete[] i;
    }
}
}
