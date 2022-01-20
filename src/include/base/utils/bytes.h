#pragma once

#include "base/trcdef.h"
#include <cstdint>

namespace trc::utils {
/**
 * @brief 改变字节序：大端改成小端，小端改成大端
 * @param bytes 需要转换的起点
 * @param size 需要转换的字节数
 * 注：之所以使用unsigned char，是因为unsigned
 * char正好是一个字节，而且比较好操作，使用之前需要把指针转为（char*）
 */
TRC_base_func_api void bytes_order_change(
    def::byte_t* const& bytes_, int size);

uint32_t utf_8_decode();

uint32_t utf_8_encode();
}
