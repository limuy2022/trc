#include "platform.h"

using namespace std;

bool get_byte_order() {
    /**
     * 获取字节序，true代表大端，false代表小端
     */
    int tmp = 0;
    char * p = (char*)(&tmp);
    return !(*p)? true: false;
}

bool byte_order = get_byte_order();
