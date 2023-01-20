#pragma once

#include <base/trcdef.hpp>
#include <cstdint>

namespace trc::utils {
/**
 * @brief 改变字节序：大端改成小端，小端改成大端
 * @param bytes_ 需要转换的数组首地址
 * @param size 数组长度
 */
void bytes_order_change(def::byte_t* const& bytes_, size_t size);
}
