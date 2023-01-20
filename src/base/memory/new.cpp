/**
 * 管理内存申请
 */

#include <base/Error.hpp>
#include <base/memory/memory.hpp>
#include <language/error.hpp>
#include <new>

namespace trc::memory {
/**
 * @brief 当没有内存时报错
 */
static void nomemory() {
    error::send_error(error::MemoryError, language::error::memoryerror);
}

void init_mem() {
    // 设置内存不足时的处理函数
    std::set_new_handler(nomemory);
}
}
