#ifndef TRC_INCLUDE_MEM_H
#define TRC_INCLUDE_MEM_H

/**
 * 管理trc的内存
 * 当申请失败时报出MemoryError
 */

#include <map>
#include "../TVM/base.h"
#include "../TVM/int.h"
#include "../TVM/float.h"
#include "../TVM/bignum.h"
#include "../TVM/string.h"
#include "memory_pool.h"

using namespace std;

class TVM;

class treenode;

class gc_obj;

class objs_pool_TVM;

void free_tree(const treenode *head);

void free_module(map<string, TVM *> &modules);

void free_TVM(TVM *vm);

void init_mem();

void quit_mem();

void free_var_vm(TVM *vm);

template<typename T> class objs_pool;

class objs_pool_TVM {
    /**
     * 对对象池的基本封装，对象池面向全体对象
     * 而这个类面向TVM用于保存基础类型
     */ 
public:
    objs_pool_TVM(gc_obj* con);
    ~objs_pool_TVM();

    objs_pool<trcint>* int_pool;
    objs_pool<trcfloat>* float_pool;
    objs_pool<trc_string>* str_pool;
    objs_pool<BigNum>* long_pool;
};

class gc_obj {
    /**
     * gc对象，对应一台虚拟机(全局内存池除外)，记录了每个对象的引用计数
     * (没有虚拟机也没有关系，并不关心调用者)
     */ 

public:
    gc_obj();

    void gc();

    ~gc_obj();

    // 通过指针链接双方对象进行管理
    // 具体流程为objs_pool无法申请内存，然后向gc_obj发出gc指令对它进行回收
    objs_pool_TVM* pool_;
};

namespace memory {
    /**
     * 存放内存池和对象池
     */ 
    extern gc_obj* global_gc_obj;
    extern objs_pool_TVM* global_objs_pool;
    extern memory_pool* global_memory_pool;
}

#endif
