#include <base/color.h>
#include <platform.h>
#ifdef WINDOWS_PLAT
#include <windows.h>
#endif

namespace trc::color {
color_st yellow(YELLOW), blue(BLUE), red(RED), green(GREEN), white(WHITE),
    purple(PURPLE);

#ifdef WINDOWS_PLAT
void console_init() {
    // 输出句柄
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    // 获取原本控制台输出模式
    DWORD dwOutMode;
    GetConsoleMode(hOut, &dwOutMode);
    // 开启虚拟终端，使用ANSI转义序列
    SetConsoleMode(hOut, dwOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif
}
