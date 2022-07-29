/**
 * 管理trc的内存
 * 当申请失败时报出MemoryError
 */

#pragma once

#include <base/library.h>

namespace trc::memory {
/**
 * @brief 内存申请初始化，程序启动时执行
 */
TRC_base_c_api void init_mem();
}
