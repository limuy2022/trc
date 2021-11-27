#pragma once

#include "platform.h"

#ifdef WINDOWS_PLAT
#ifdef BUILD_TVM_DLL
#define TRC_TVM_api __declspec(dllexport)
#else
#define TRC_TVM_api __declspec(dllimport)
#endif
#else
#define TRC_TVM_api
#endif
