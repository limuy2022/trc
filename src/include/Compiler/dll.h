#pragma once

#include "platform.h"

#ifdef WINDOWS_PLAT
#ifdef BUILD_Compiler_DLL
#define TRC_Compiler_api __declspec(dllexport)
#else
#define TRC_Compiler_api __declspec(dllimport)
#endif
#else
#define TRC_Compiler_api
#endif