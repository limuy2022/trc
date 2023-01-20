/**
 * 管理trc的内存
 * 当申请失败时报出MemoryError
 */

#pragma once

namespace trc::memory {
/**
 * @brief 内存申请初始化，程序启动时执行
 */
void init_mem();
}
