#pragma once

#include <platform.h>

#ifdef WINDOWS_PLAT
#ifdef BUILD_language_DLL
#define TRC_language_api __declspec(dllexport)
#else
#define TRC_language_api __declspec(dllimport)
#endif
#else
#define TRC_language_api                                   \
    __attribute__((visibility("default")))
#endif
