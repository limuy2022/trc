#pragma once

#include <platform.h>

#ifdef shared_lib
#ifdef WINDOWS_PLAT
#ifdef BUILD_Compiler_DLL
#define TRC_Compiler_api __declspec(dllexport)
#define TRC_Compiler_func_api extern "C" __declspec(dllexport)
#else
#define TRC_Compiler_api __declspec(dllimport)
#define TRC_Compiler_func_api extern "C" __declspec(dllimport)
#endif
#else
#define TRC_Compiler_api __attribute__((visibility("default")))
#define TRC_Compiler_func_api extern "C" __attribute__((visibility("default")))
#endif
#else
#define TRC_Compiler_api
#define TRC_Compiler_func_api
#endif
