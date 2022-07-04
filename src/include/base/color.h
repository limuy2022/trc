/**
 * 用于输出有颜色的字体
 */

#pragma once

#include <base/library.h>
#include <cstdio>
#include <platform.h>

/* 用于标识颜色 */
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define WHITE "\033[37m"
#define PURPLE "\033[35m"
#define CLOSE_COLOR "\033[0m"
typedef const char* color_type;

namespace trc::color {
#ifdef WINDOWS_PLAT
/**
 * @brief 初始化控制台，缓存一些数据
 */
TRC_base_c_api void console_init();

/**
 * @brief 还原控制台，使它还原到使用trc之前的状态
 */
TRC_base_c_api void color_quit();

/**
 * @brief 设置控制台为ANSI模式
 */
TRC_base_c_api void color_init();
#endif

class TRC_base_api color_st {
public:
    color_st(color_type color);

    /**
     * @brief 输出
     * @details 像使用printf一样使用它
     */
    template <typename... argvs>
    void operator()(const char* fmt, const argvs&... data) const;

private:
    color_type color;
};

template <typename... argvs>
void color_st::operator()(const char* fmt, const argvs&... data) const {
#ifdef WINDOWS_PLAT
    color_init();
#endif
    printf(this->color);
    printf(fmt, data...);
    printf(CLOSE_COLOR);
#ifdef WINDOWS_PLAT
    color_quit();
#endif
}

TRC_base_c_api color_st yellow, blue, red, green, white, purple;
}
