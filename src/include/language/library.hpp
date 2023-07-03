#pragma once

#ifdef _WIN32
#ifdef BUILD_language_DLL
#define TRC_language_c_api extern "C" __declspec(dllexport)
#else
#define TRC_language_c_api extern "C" __declspec(dllimport)
#endif
#else
#define TRC_language_c_api extern "C" __attribute__((visibility("default")))
#endif
