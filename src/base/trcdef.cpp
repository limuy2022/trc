﻿/**
 * @file trcdef.cpp
 * @brief 基础定义
 * @date 2022-04-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "base/trcdef.h"
#include "language/error.h"
#include <cstdio>
#include <cstdlib>

namespace trc {
namespace def {
    const float version = 0.2;
}

void noreach_internal(size_t line, const char* funcname,
    const char* filename) {
    fprintf(stderr, "Fatal error in %s %s %zu\n%s",
        filename, funcname, line, language::error::noreach);
    exit(1);
}
}