/**
 * 进度条
 */
#include <base/trcdef.h>
#include <base/utils/data.hpp>
#include <base/utils/progress_bar.h>
#include <cstdio>

namespace trc::utils {
progress_bar::progress_bar(
    unsigned int total_steps, char background, char bar_char)
    : total_steps(total_steps)
    , background(background)
    , bar_char(bar_char) {
    for (int i = 0; i < 100; ++i) {
        putchar(background);
    }
    this->draw();
}
void progress_bar::next() {
    if (now_progress > total_steps) {
        // 到达此处说明代码出现错误
        NOREACH;
    }
    now_progress++;
}

void progress_bar::draw() const {
    int precent = int(100 * (now_progress * 1.0 / total_steps));
    printf("\r[%d%%]", precent);
    // 打印100减去中括号和百分号再减去数字
    for (unsigned int i = 0;
         i < 100 * (now_progress * 1.0 / total_steps) - 3 - utils::len(precent);
         ++i) {
        putchar(bar_char);
    }
}
}
