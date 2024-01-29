/**
 * 用于输出有颜色的字体
 */

module;
#include <cstdio>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
export module color;

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
#ifdef _WIN32
/**
 * @brief 初始化控制台，设置控制台为ANSI模式
 */
export void console_init() {
    // 输出句柄
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    // 获取原本控制台输出模式
    DWORD dwOutMode;
    GetConsoleMode(hOut, &dwOutMode);
    // 开启虚拟终端，使用ANSI转义序列
    SetConsoleMode(hOut, dwOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

export class color_st {
public:
    explicit color_st(color_type color)
        : color(color) {
    }

    /**
     * @brief 输出
     * @details 像使用printf一样使用它
     */
    void operator()(const std::string& output_msg) const;

private:
    color_type color;
};

void color_st::operator()(const std::string& output_msg) const {
    printf("%s", this->color);
    printf("%s", output_msg.c_str());
    printf("%s", CLOSE_COLOR);
}

export color_st yellow(YELLOW), blue(BLUE), red(RED), green(GREEN),
    white(WHITE), purple(PURPLE);
}
