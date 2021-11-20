#include "platform.h"

using namespace std;

constexpr bool get_byte_order() {
    /**
     * 获取字节序，true代表大端，false代表小端
     */
    int tmp = 0;
    return !(*(char*)(&tmp));
}

const bool byte_order = get_byte_order();
