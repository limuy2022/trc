#ifndef TRC_INCLUDE_MEMORY_MEM_H
#define TRC_INCLUDE_MEMORY_MEM_H

/**
 * 管理trc的内存
 * 当申请失败时报出MemoryError
 */

#include <map>
#include "objs_pool.hpp"
#include "TVM/base.h"
#include "TVM/int.h"
#include "TVM/float.h"
#include "TVM/long.h"
#include "TVM/string.h"
#include "TVM/flong.h"
#include "memory_pool.h"

// 申请对象池
#define MALLOCINT memory::global_objs_pool->int_pool.trcmalloc
#define MALLOCFLOAT memory::global_objs_pool->float_pool.trcmalloc
#define MALLOCSTRING memory::global_objs_pool->str_pool.trcmalloc
#define MALLOCLONG memory::global_objs_pool->long_pool.trcmalloc
#define MALLOCFLONG memory::global_objs_pool->flong_pool.trcmalloc

// 申请内存池
#define MALLOC memory::global_memory_pool.mem_malloc
#define REALLOC memory::global_memory_pool.mem_realloc
#define FREE memory::global_memory_pool.mem_free

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

struct objs_pool_TVM {
    /**
     * 对对象池的基本封装，对象池面向全体对象
     * 而这个类面向TVM用于保存基础类型
     */
    objs_pool<trc_int> int_pool;
    objs_pool<trc_float> float_pool;
    objs_pool<trc_string> str_pool;
    objs_pool<trc_long> long_pool;
    objs_pool<trc_flong> flong_pool;
};

namespace memory {
    /**
     * 存放内存池和对象池
     */
    extern memory_pool global_memory_pool;
    extern objs_pool_TVM * global_objs_pool;
}

#endif
