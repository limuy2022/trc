#pragma once

#include <base/trcdef.h>
#include <cstdint>

namespace trc::utils {
/**
 * @brief 改变字节序：大端改成小端，小端改成大端
 * @param bytes 需要转换的起点
 * @param size 需要转换的字节数
 */
void bytes_order_change(def::byte_t* const& bytes_, size_t size);
}
