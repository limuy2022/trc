﻿/**
 * 跨平台需要使用的头文件
 *
 * 同时在此进行一些校验
 */

#pragma once

#if defined(WIN32) || defined(_WIN32)                      \
    || defined(__WIN32__) || defined(__NT__)
#define WINDOWS_PLAT
#elif defined(__linux__)
#define LINUX_PLAT
#else
// 不受支持的平台，报错
#error Cannot compile on this platform
#endif

namespace trc {
namespace utils {
    /**
     * @brief 获取字节序
     * @return true代表大端，false代表小端
     */
    bool get_byte_order();
}
namespace def {
    // 字节序，true为大端，false为小端
    extern const bool byte_order;
}
}
