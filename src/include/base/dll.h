#pragma once

#include "platform.h"

#ifdef WINDOWS_PLAT
    #ifdef BUILD_base_DLL
        #define TRC_base_api __declspec(dllexport)
    #else
        #define TRC_base_api __declspec(dllimport)
    #endif
#else
    #define TRC_base_api
#endif
