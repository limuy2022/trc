#include <base/color.h>
#include <platform.h>
#ifdef WINDOWS_PLAT
#include <windows.h>

static DWORD dwOutMode_commom, dwOutMode_color;
//=输出句柄
static HANDLE hOut;
#endif

namespace trc::color {
color_st yellow(YELLOW), blue(BLUE), red(RED), green(GREEN), white(WHITE),
    purple(PURPLE);

color_st::color_st(color_type color)
    : color(color) {
}

#ifdef WINDOWS_PLAT
void console_init() {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    //获取原本控制台输出模式
    GetConsoleMode(hOut, &dwOutMode_commom);
    dwOutMode_color = dwOutMode_commom | 0x0004;
}

void color_init() {
    //设置控制台输出模式
    SetConsoleMode(hOut, dwOutMode_color);
}

void color_quit() {
    SetConsoleMode(hOut, dwOutMode_commom);
}
#endif
}
