#include <base/utils/bytes.h>
#include <cassert>

namespace trc::utils {
void bytes_order_change(def::byte_t* const& bytes_, size_t size) {
    size_t end = size - 1;
    size_t start = 0;
    for (size_t i = 0, cnt = size / 2; i < cnt; i++) {
        std::swap(bytes_[start + i], bytes_[end - i]);
    }
}

int utf_8_decode(const uint8_t* bytePtr, uint32_t length) {
    //若是1字节的ascii:  0xxxxxxx
    if (*bytePtr <= 0x7f) {
        return *bytePtr;
    }

    int value;
    uint32_t remainingBytes;

    //先读取高1字节
    //根据高字节的高n位判断相应字节数的utf8编码
    if ((*bytePtr & 0xe0) == 0xc0) {
        //若是2字节的utf8
        value = *bytePtr & 0x1f;
        remainingBytes = 1;
    } else if ((*bytePtr & 0xf0) == 0xe0) {
        //若是3字节的utf8
        value = *bytePtr & 0x0f;
        remainingBytes = 2;
    } else if ((*bytePtr & 0xf8) == 0xf0) {
        //若是4字节的utf8
        value = *bytePtr & 0x07;
        remainingBytes = 3;
    } else {   //非法编码
        return -1;
    }

    //如果utf8被斩断了就不再读过去了
    if (remainingBytes > length - 1) {
        return -1;
    }

    //再读取低字节中的数据
    while (remainingBytes > 0) {
        bytePtr++;
        remainingBytes--;
        //高2位必须是10
        if ((*bytePtr & 0xc0) != 0x80) {
            return -1;
        }

        //从次高字节往低字节,不断累加各字节的低6位
        value = value << 6 | (*bytePtr & 0x3f);
    }
    return value;
}

uint32_t utf_8_encode(uint8_t* buf, int value) {
    assert(value > 0);

    //按照大端字节序写入缓冲区
    if (value <= 0x7f) {   // 单个ascii字符需要1字节
        *buf = value & 0x7f;
        return 1;
    } else if (value <= 0x7ff) {  //此范围内数值编码为utf8需要2字节
        //先写入高字节
        *buf++ = 0xc0 | ((value & 0x7c0) >> 6);
        // 再写入低字节
        *buf = 0x80 | (value & 0x3f);
        return 2;
    } else if (value <= 0xffff) { //此范围内数值编码为utf8需要3字节
        // 先写入高字节
        *buf++ = 0xe0 | ((value & 0xf000) >> 12);
        //再写入中间字节
        *buf++ = 0x80 | ((value & 0xfc0) >> 6);
        //最后写入低字节
        *buf = 0x80 | (value & 0x3f);
        return 3;
    } else if (value <= 0x10ffff) { //此范围内数值编码为utf8需要4字节
        *buf++ = 0xf0 | ((value & 0x1c0000) >> 18);
        *buf++ = 0x80 | ((value & 0x3f000) >> 12);
        *buf++ = 0x80 | ((value & 0xfc0) >> 6);
        *buf = 0x80 | (value & 0x3f);
        return 4;
    }
    NOREACH("Can't encode %d", value);
    return 0;
}

uint32_t getByteNumOfEncodeUtf8(int value){
    assert(value > 0);
    // 单个ascii字符需要1字节
    if (value <= 0x7f) {
        return 1;
    }
    if (value <= 0x7ff) {
        return 2;
    }
    if (value <= 0xffff) {
        return 3;
    }
    if (value <= 0x10ffff) {
        return 4;
    }
    //超过范围返回0
    return 0;
}
}
