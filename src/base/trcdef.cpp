/**
 * @file trcdef.cpp
 * @brief 基础定义
 * @date 2022-04-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <base/trcdef.hpp>
#include <cstdio>
#include <cstdlib>
#include <language/error.hpp>

namespace trc {
void noreach_internal(size_t line, const char* funcname, const char* filename) {
    fprintf(stderr,
        "\nFatal error in function \"%s\" file %s line "
        "%zu\n%s",
        funcname, filename, line, language::error::noreach);
    exit(EXIT_FAILURE);
}
}
