/**
 * 用于输出有颜色的字体
 */

#pragma once

#include <base/library.h>
#include <cstdio>
#include <platform.h>

// windows和linux输出颜色的方式不同，这种为linux的
#ifdef LINUX_PLAT
/* 用于标识颜色 */
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define WHITE "\033[37m" /*虽然默认就是白色，但终端颜色是可以手动调整的*/
#define PURPLE "\033[35m"
#define CLOSE_COLOR "\033[0m"
typedef const char* color_type;
#elif defined(WINDOWS_PLAT)

#include <windows.h>

#define RED (FOREGROUND_INTENSITY | FOREGROUND_RED)
#define GREEN (FOREGROUND_INTENSITY | FOREGROUND_GREEN)
#define YELLOW (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN)
#define BLUE (FOREGROUND_INTENSITY | FOREGROUND_BLUE)
#define WHITE                                                                  \
    (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN                  \
        | FOREGROUND_BLUE) /*虽然默认就是白色，但终端颜色是可以手动调整的*/
#define PURPLE (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE)
#define CLOSE_COLOR WHITE
typedef int color_type;
#endif

namespace trc::color {
#ifdef WINDOWS_PLAT
TRC_base_c_api HANDLE hout;
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
    SetConsoleTextAttribute(hout, this->color);
    printf(fmt, data...);
    SetConsoleTextAttribute(hout, CLOSE_COLOR);
#elif defined(LINUX_PLAT)
    printf(this->color);
    printf(fmt, data...);
    printf(CLOSE_COLOR);
#endif
}

TRC_base_c_api color_st yellow, blue, red, green, white, purple;
}
