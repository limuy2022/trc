module;
#include <algorithm>
#include <cstdint>
export module bytes;
import trcdef;

namespace trc::utils {
/**
 * @brief 改变字节序：大端改成小端，小端改成大端
 * @param bytes_ 需要转换的数组首地址
 * @param size 数组长度
 */
export void bytes_order_change(def::byte_t* const& bytes_, size_t size) {
    for (size_t i = 0; i < size / 2; i++) {
        std::swap(bytes_[i], bytes_[size - 1 - i]);
    }
}
}
