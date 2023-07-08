﻿/**
 * 进度条
 */

module;
#include <cstdio>
#include <unreach.hpp>
export module progress_bar;
import trcdef;
import data;

namespace trc::utils {
/**
 * @brief 用于显示进度条
 * 绘制风格:
 * [xxx%]:------------======================
 */
export class progress_bar {
public:
    progress_bar(unsigned int total_steps, char background, char bar_char);

    /**
     * @brief 进度+1,(延长进度条)
     * @warning 调用该函数后不会自动绘制新进度条
     */
    void next();

    /**
     * @brief 绘制进度条
     */
    void draw() const;

private:
    // 进度条总长度
    unsigned int total_steps;
    // 当前进度
    unsigned int now_progress = 0;
    // 进度条背景字符
    char background;
    // 进度条字符
    char bar_char;
};

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
        UNREACH("The progress %u has exceeded the total progress %u",
            now_progress, total_steps);
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