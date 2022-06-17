#pragma once

#include <platform.h>

#ifdef WINDOWS_PLAT
#ifdef BUILD_TVMbase_DLL
#define TRC_TVMbase_api __declspec(dllexport)
#define TRC_TVMbase_func_api extern "C" __declspec(dllexport)
#else
#define TRC_TVMbase_api __declspec(dllimport)
#define TRC_TVMbase_func_api extern "C" __declspec(dllimport)
#endif
#else
#define TRC_TVMbase_api __attribute__((visibility("default")))
#define TRC_TVMbase_func_api extern "C" __attribute__((visibility("default")))
#endif