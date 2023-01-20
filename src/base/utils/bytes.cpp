#include <base/utils/bytes.hpp>

namespace trc::utils {
void bytes_order_change(def::byte_t* const& bytes_, size_t size) {
    for (size_t i = 0; i < size / 2; i++) {
        std::swap(bytes_[i], bytes_[size - 1 - i]);
    }
}
}
