/**
 * 管理内存申请
 */

#include <base/Error.h>
#include <base/memory/memory.h>
#include <base/memory/memory_pool.h>
#include <language/error.h>
#include <new>

namespace trc::memory {
/**
 * @brief 当没有内存时报错
 */
static void nomemory() {
    error::send_error(error::MemoryError, language::error::memoryerror);
}

memory_pool global_memory_pool;

void init_mem() {
    // 设置内存不足时的处理函数
    std::set_new_handler(nomemory);
}
}
