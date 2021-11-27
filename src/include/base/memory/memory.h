#pragma once

/**
 * 管理trc的内存
 * 当申请失败时报出MemoryError
 */

#include "base/dll.h"
#include "memory_pool.h"

// 申请内存池
#define MALLOC trc::memory::global_memory_pool.mem_malloc
#define REALLOC trc::memory::global_memory_pool.mem_realloc
#define FREE trc::memory::global_memory_pool.mem_free

using namespace std;

namespace trc {
    namespace memory {
        TRC_base_api void init_mem();
        
        TRC_base_api void quit_mem();

        TRC_base_api extern memory_pool global_memory_pool;
    }
}
