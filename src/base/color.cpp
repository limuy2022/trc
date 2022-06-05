#include <base/color.h>

namespace trc::color {
#ifdef WINDOWS_PLAT
HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
color_st yellow(YELLOW), blue(BLUE), red(RED), green(GREEN),
    white(WHITE), purple(PURPLE);

color_st::color_st(color_type color)
    : color(color) {
}
}
