#pragma once

/**
 * 管理trc的内存
 * 当申请失败时报出MemoryError
 */

#include <base/library.h>
#include <base/memory/memory_pool.h>

// 申请内存池
#define MALLOC trc::memory::global_memory_pool.mem_malloc
#define REALLOC trc::memory::global_memory_pool.mem_realloc
#define FREE trc::memory::global_memory_pool.mem_free

namespace trc::memory {
/**
 * @brief 内存申请初始化，程序启动时执行
 */
TRC_base_c_api void init_mem();

/**
 * @brief 释放内存，程序结束时使用
 */
TRC_base_c_api void quit_mem();

/**
 * @brief 管理虚拟机对象分配内存的内存池
 */
TRC_base_c_api extern memory_pool global_memory_pool;
}
