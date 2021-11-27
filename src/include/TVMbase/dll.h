#pragma once

#include "platform.h"

#ifdef WINDOWS_PLAT
    #ifdef BUILD_TVMbase_DLL
        #define TRC_TVMbase_api __declspec(dllexport)
    #else
        #define TRC_TVMbase_api __declspec(dllimport)
    #endif
#else
    #define TRC_TVMbase_api
#endif
