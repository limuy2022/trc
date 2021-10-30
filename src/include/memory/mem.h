#ifndef TRC_INCLUDE_MEMORY_MEM_H
#define TRC_INCLUDE_MEMORY_MEM_H

/**
 * 管理trc的内存
 * 当申请失败时报出MemoryError
 */

#include <map>
#include "TVM/base.h"
#include "TVM/int.h"
#include "TVM/float.h"
#include "TVM/long.h"
#include "TVM/string.h"
#include "memory_pool.h"

using namespace std;

class TVM;

class treenode;

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
    objs_pool_TVM(size_t init_size);
    ~objs_pool_TVM();
    gc();

    objs_pool<trc_int>* int_pool;
    objs_pool<trc_float>* float_pool;
    objs_pool<trc_string>* str_pool;
    objs_pool<trc_long>* long_pool;
};

namespace memory {
    /**
     * 存放内存池和对象池
     */ 
    extern objs_pool_TVM* global_objs_pool;
    extern memory_pool* global_memory_pool;
}

#endif
