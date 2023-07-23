/**
 * 管理trc的内存
 * 当申请失败时报出MemoryError
 */

module;
#include <new>
export module memory;
import Error;
import language;

namespace trc::memory {
/**
 * @brief 当没有内存时报错
 */
static void nomemory() {
    error::send_error(error::MemoryError, language::error::memoryerror);
}

/**
 * @brief 内存申请初始化，程序启动时执行
 */
export void init_mem() {
    // 设置内存不足时的处理函数
    std::set_new_handler(nomemory);
}
}
