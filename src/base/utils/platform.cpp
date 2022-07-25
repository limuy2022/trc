#include <platform.h>

namespace trc {
namespace utils {
    bool get_byte_order() {
        int tmp = 0;
        return !(*(char*)(&tmp));
    }
}

namespace def {
    const bool byte_order = utils::get_byte_order();
}
}
