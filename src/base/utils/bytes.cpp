#include <base/utils/bytes.h>

namespace trc::utils {
void bytes_order_change(
    def::byte_t* const& bytes_, size_t size) {
    size_t end = size - 1;
    size_t start = 0;
    for (size_t i = 0, cnt = size / 2; i < cnt; i++) {
        std::swap(bytes_[start + i], bytes_[end - i]);
    }
}

uint32_t utf_8_decode() {
    return uint32_t();
}

uint32_t utf_8_encode() {
    return uint32_t();
}
}
