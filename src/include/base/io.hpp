/**
 * @file io.hpp
 * @brief 输入输出
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include <cstdio>

namespace trc::io {
/**
 * @brief 读入一个长度不限的字符串(不包括换行符)
 * @details 具体使用可以参考getline(cin, str)函数的使用
 * @warning
 * 参数必须是未分配内存的，已分配的需要在进函数之前释放掉
 * @return true:正确读取,没有读取到尾部，false:读取完毕，已经读取到文件流尾部
 */
bool readstr(char*&, FILE* stream = stdin);
}
