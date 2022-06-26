/**
 * @file easter_eggs.h
 * @brief 彩蛋接口
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 */

#include <base/library.h>
#include <vector>

namespace trc::eggs {
/**
 * @brief 表示一个物体的坐标
 */
class TRC_base_api pos {
public:
    int count_;
    int high = 0;
    pos(int count);
};

typedef std::vector<std::vector<pos>> canvas_data;

/**
 * @brief 画方块图的函数
 * @param pos_ 描述画布数据
 * @details
 * 数据描述方法：
 * 在第i行j列的位置放上a个方格
 * 例如:
 * {{1,1},
 * {3,2}}
 * 意思是在(1,1)位置放上一个方格，
 * (1, 2)位置放上一个方格,
 * (2, 1)位置放上三个方格,
 * (2, 2)位置放上两个方格
 * @warning
 * 这个函数将会修改pos_参数中的值，理想状态下，最后pos_应该全部被清零
 */
TRC_base_c_api void draw_pictures(canvas_data& pos_, char background = '.');
}
