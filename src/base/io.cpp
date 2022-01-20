/**
 * @file io.cpp
 * @brief 输入输出函数
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "base/io.hpp"
#include "base/Error.h"
#include "language/error.h"
#include <cstdlib>

namespace trc {
namespace io {
    void readstr(char*& str) {
        size_t len = 15;
        str = (char*)malloc((len + 1) * sizeof(char));
        if (str == nullptr) {
            error::send_error(error::MemoryError,
                language::error::memoryerror);
        }
        size_t index = 0;
        char c = getchar();
        while (c != '\n') {
            if (index > len) {
                len += 15;
                str = (char*)realloc(
                    str, (len + 1) * sizeof(char));
                if (str == nullptr) {
                    error::send_error(error::MemoryError,
                        language::error::memoryerror);
                }
            }
            str[index] = c;
            c = getchar();
            index++;
        }
        str[index] = '\0';
    }
}
}
