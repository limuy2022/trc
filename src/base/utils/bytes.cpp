#include "base/utils/bytes.h"

namespace trc::utils {
void bytes_order_change(
    def::byte_t* const& bytes_, int size) {
    int end = size - 1;
    def::byte_t tmp;
    int start = 0;
    for (int i = 0, cnt = size / 2; i < cnt; i++) {
        tmp = bytes_[start + i];
        bytes_[start + i] = bytes_[end - i];
        bytes_[end - i] = tmp;
    }
}

uint32_t utf_8_decode() {
    return uint32_t();
}

uint32_t utf_8_encode() {
    return uint32_t();
}
}
