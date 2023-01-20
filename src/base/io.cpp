﻿/**
 * @file io.cpp
 * @brief 输入输出函数
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <base/Error.hpp>
#include <base/io.hpp>
#include <cstdio>
#include <cstdlib>
#include <language/error.hpp>

const size_t mem_init_size = 15;
const size_t mem_realloc_size = 20;

namespace trc::io {
bool readstr(char*& str, FILE* stream) {
    size_t len = mem_init_size;
    str = (char*)malloc((len + 1) * sizeof(char));
    if (str == nullptr) {
        error::send_error(error::MemoryError, language::error::memoryerror);
    }
    size_t index = 0;
    // 采用int是为了检测出EOF标志
    int c = fgetc(stream);
    while (c != '\n') {
        if (c == EOF) {
            // 文件读取完毕
            str[index] = '\0';
            return false;
        }
        if (index > len) {
            len += mem_realloc_size;
            str = (char*)realloc(str, (len + 1) * sizeof(char));
            if (str == nullptr) {
                error::send_error(
                    error::MemoryError, language::error::memoryerror);
            }
        }
        str[index] = (char)c;
        c = fgetc(stream);
        index++;
    }
    str[index] = '\0';
    return true;
}
}
