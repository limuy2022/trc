#pragma once

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <language/error.hpp>

/**
 * @param ... 报错的辅助输出信息
 */
#define UNREACH(...)                                                           \
    do {                                                                       \
        fprintf(stderr, __VA_ARGS__);                                          \
        trc::unreach_internal(__LINE__, __FUNCTION__, __FILE__);               \
    } while (0)

namespace trc {
inline void unreach_internal(
    size_t line, const char* funcname, const char* filename) {
    fprintf(stderr,
        "\nFatal error in function \"%s\" file %s line "
        "%zu\n%s",
        funcname, filename, line, language::error::noreach);
    exit(EXIT_FAILURE);
}
}
