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
 * @return true:正确读取，false:读取完毕(字符串可以正常使用)
 */
bool readstr(char*&, FILE* stream = stdin);

// 用来占位的变量,不要读取，可能是无意义的值
extern bool readflagdefault;

/**
 * @brief 快速读入一个整数，当从文件读入时性能比scanf高
 * @param stream 输入流
 * @param readflag 读入标志，标志是否正确，false为错误
 * @tparam T 整数类型int，long等
 */
template <typename T>
T fast_int_read(FILE* stream = stdin, bool& readflag = readflagdefault) {
    T res = 0;
    int f = 1;
    int c;
    for (;;) {
        c = fgetc(stream);
        if ('0' <= c && c <= '9') {
            break;
        }
        if (c == EOF) {
            readflag = false;
            return 0;
        }
        if (c == '-') {
            f = -1;
        }
    }
    do {
        res *= 10;
        res += c - '0';
        c = fgetc(stream);
    } while ('0' <= c && c <= '9');
    return res * f;
}

/**
 * @brief 快速输出的内部真正实现
 * @param stream 输出流
 * @details 设置两层包装是为了提高性能，减少无用判断
 */
template <typename T>
static inline void fast_int_write_internal(T num, FILE* stream) {
    if (num < 10) {
        fputc(num + '0', stream);
    } else {
        fast_int_write_internal(num / 10, stream);
        fputc(num % 10 + '0', stream);
    }
}

/**
 * @brief 快速输出一个整数，当向文件输出性能比printf要高
 * @param stream 输出流
 * @param num 整数
 */
template <typename T> void fast_int_write(T num, FILE* stream = stdout) {
    if (num < 0) {
        fputc('-', stream);
        fast_int_write_internal(-num, stream);
    } else {
        fast_int_write_internal(num, stream);
    }
}
}
