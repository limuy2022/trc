#pragma once

#include <platform.hpp>

#ifdef WINDOWS_PLAT
#ifdef BUILD_language_DLL
#define TRC_language_c_api extern "C" __declspec(dllexport)
#else
#define TRC_language_c_api extern "C" __declspec(dllimport)
#endif
#else
#define TRC_language_c_api extern "C" __attribute__((visibility("default")))
#endif
