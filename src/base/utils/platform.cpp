#include <cstdint>
#include <platform.h>

namespace trc {
namespace utils {
    bool get_byte_order() {
        const int32_t tmp = 1;
        return !(*(char8_t*)(&tmp));
    }
}

namespace def {
    const bool byte_order = utils::get_byte_order();
}
}
