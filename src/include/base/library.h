#pragma once

#include <platform.h>

#ifdef shared_lib
#ifdef WINDOWS_PLAT
#ifdef BUILD_base_DLL
#define TRC_base_api __declspec(dllexport)
#define TRC_base_c_api extern "C" __declspec(dllexport)
#else
#define TRC_base_api __declspec(dllimport)
#define TRC_base_c_api extern "C" __declspec(dllimport)
#endif
#else
#define TRC_base_api __attribute__((visibility("default")))
#define TRC_base_c_api extern "C" __attribute__((visibility("default")))
#endif
#else
#define TRC_base_api
#define TRC_base_c_api
#endif
