﻿/**
 * @file autodll.h
 * @brief 动态加载dll的接口
 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 */

#include "dll.h"
#include "platform.h"

#ifdef WINDOWS_PLAT
#include <windows.h>
#elif defined(LINUX_PLAT)
#include <dlfcn.h>
#endif

namespace trc {
namespace autodll {
// 定义加载出的动态链接库的储存类型
#ifdef WINDOWS_PLAT
    typedef HINSTANCE dll_t;
#elif defined(LINUX_PLAT)
    typedef void* dll_t;
#endif

    /**
     * @brief 打开一个动态链接库
     * @return dll
     */
    TRC_base_func_api dll_t dllopen(const char*);

    /**
     * @brief 关闭释放一个动态链接库
     */
    TRC_base_func_api void dllfree(dll_t dll);

    /**
     * @brief 从动态链接库中加载出一个函数
     * @param dll dll
     * @param funcname 函数名
     */
    TRC_base_func_api void* dllfuncload(
        dll_t dll, const char* funcname);
}
}