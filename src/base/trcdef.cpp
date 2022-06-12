/**
 * @file trcdef.cpp
 * @brief 基础定义
 * @date 2022-04-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <base/trcdef.h>
#include <cstdio>
#include <cstdlib>
#include <language/error.h>

namespace trc {
namespace def {
    const float version = 0.2;
    const char* repo_github
        = "https://github.com/limuy2022/trc.git";
    const char* repo_gitee
        = "https://gitee.com/li-muyangangel/trc.git";
}

void noreach_internal(size_t line, const char* funcname,
    const char* filename) {
    fprintf(stderr,
        "Fatal error in function \"%s\" file %s line "
        "%zu\n%s",
        funcname, filename, line, language::error::noreach);
    exit(1);
}
}