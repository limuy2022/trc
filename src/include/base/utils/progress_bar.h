#pragma once

#include <base/library.h>

namespace trc::utils {
/**
 * @brief 用于显示进度条
 * 绘制风格:
 * [xxx%]:------------======================
 */
class TRC_base_api progress_bar {
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
}
