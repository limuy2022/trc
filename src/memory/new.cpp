/**
 * 管理内存申请
 */

#include <new>
#include "../include/Error.h"
#include "../include/TVM/TRE.h"
#include "../include/TVM/int.h"
#include "../include/TVM/float.h"
#include "../include/TVM/bignum.h"
#include "../include/TVM/string.h"
#include "../include/memory/mem.h"
#include "../include/memory/memory_pool.h"

using namespace std;

namespace memory {
    /**
     * 存放内存池和对象池
     */ 
    gc_obj* global_gc_obj(new gc_obj);
    objs_pool_TVM* global_objs_pool(new objs_pool_TVM(global_gc_obj));
    memory_pool* global_memory_pool(new memory_pool);
}

void nomemory() {
    /**
    * 当没有内存时报错
    */
    send_error(MemoryError, "can't get the memory from os.");
}

void init_mem() {
    /**
     * 内存申请初始化，程序启动时执行
     * 初始化整型和布尔缓存
     */

    set_new_handler(nomemory);
    // 布尔值初始化
    TVM_share::true_ = new trcint(1), TVM_share::false_ = new trcint(0);
    // 整型缓存初始化，-5~256
    int index = 0;
    for(int i = INT_CACHE_BEGIN; i <= INT_CACHE_END; ++i)
        TVM_share::int_cache[index++] = new trcint(i);
}
