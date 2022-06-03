/**
 * @file easter_eggs.cpp
 * @brief 留的小彩蛋，可以自定义画出小方格
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 */

#include "base/easter_eggs.h"
#include <cstdio>
#include <cstring>
#include <vector>

/**
 * 默认图形：
 *   +---+
 *  /   /|
 * +---+ |
 * |   | +
 * |   |/
 * +---+
 */
static const char* block[] = { "+---+", "/   /|", "+---+ |",
    "|   | +", "|   |/", "+---+" };

inline static int realx(int x) {
    return x * 2;
}

inline static int realy(int x, int y) {
    return y * 4 - x * 2;
}

namespace trc::eggs {
/**
 * @brief 负责绘图和设置方块
 */
class drawer {
public:
    /**
     * @brief 计算出画布大小并初始化画布
     * @param canvas
     */
    drawer(const canvas_data& canvas, char background);

    ~drawer();

    /**
     * @brief 打印出方块
     */
    void draw();

    /**
     * @brief 在指定位置设置方块
     * @param x 横坐标
     * @param y 纵坐标
     */
    void set_block(int x, int y);

private:
    /**
     * @brief 检查所在行是否全部为背景标识符
     * @param x 所在行
     * @return true 是
     * @return false 不是
     */
    bool check_x(int x);

    /**
     * @brief 检查所在列是否全部为背景标识符
     * @param y 所在列
     * @return true 是
     * @return false 不是
     */
    bool check_y(int y);

    // 有多少行，用来释放内存用的
    size_t line_nums;
    // 画布具体数据
    char** canvas_;

    // 画布长
    size_t length;
    // 画布宽
    size_t width;
    // 背景字符
    char background;
};

drawer::drawer(const canvas_data& canvas, char background)
    : line_nums(canvas.size())
    , background(background) {
    // 根据数据计算画布长宽

    // 初始化数据
    for (size_t i = 0; i < length; ++i) {
        for (size_t j = 0; j < width; ++j) {
            canvas_[i][j] = background;
        }
    }
}

drawer::~drawer() {
    for (size_t i = 0; i < line_nums; ++i) {
        delete[] canvas_[i];
    }
    delete[] canvas_;
}

bool drawer::check_x(int x) {
    for (size_t i = 0; i < length; ++i) {
        if (canvas_[x][i] != background) {
            return false;
        }
    }
    return true;
}

bool drawer::check_y(int y) {
    for (size_t i = 0; i < width; ++i) {
        if (canvas_[i][y] != background) {
            return false;
        }
    }
    return true;
}

void drawer::set_block(int x, int y) {
    memcpy(&canvas_[x + 0][y + 2], block[0],
        strlen(block[0]) * sizeof(char));
    memcpy(&canvas_[x + 1][y + 1], block[1],
        strlen(block[1]) * sizeof(char));
    for (int i = 2; i < 6; ++i) {
        memcpy(&canvas_[x + i][y], block[i],
            strlen(block[i]) * sizeof(char));
    }
}

void drawer::draw() {
    int x_begin = 0, y_begin = 0, x_end = 909, y_end = 909;
    while (check_x(x_begin)) {
        x_begin++;
    }
    while (check_x(x_end)) {
        x_end--;
    }
    while (check_y(y_begin)) {
        y_begin++;
    }
    while (check_y(y_end)) {
        y_end--;
    }
    for (int i = x_begin; i <= x_end; ++i) {
        for (int j = y_begin; j <= y_end; ++j) {
            putchar(canvas_[i][j]);
        }
        putchar('\n');
    }
}

void draw_pictures(canvas_data& pos_, char background) {
    // 构建一个绘画者
    drawer painter(pos_, background);
    // 开始设置方块
    bool flag = false;
    size_t m = pos_.size(), n = pos_[0].size();
    while (1) {
        for (size_t i = 1; i <= m; ++i) {
            for (size_t j = 1; j <= n; ++j) {
                if (pos_[i][j].count_ != 0) {
                    painter.set_block(
                        realx(i) - pos_[i][j].high * 3,
                        realy(i, j));
                    pos_[i][j].count_--;
                    pos_[i][j].high++;
                    flag = true;
                }
            }
        }
        if (!flag) {
            break;
        }
        flag = false;
    }
    // 打印
    painter.draw();
}
}
