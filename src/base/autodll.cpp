/**
 * @file autodll.cpp
 * @brief 显式加载动态链接库的包装
 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 */

#include "base/autodll.h"
#include "base/Error.h"
#include "language/error.h"
#include "platform.h"

namespace trc {
namespace autodll {
// 定义加载出的动态链接库的储存类型
#ifdef WINDOWS_PLAT
    typedef HINSTANCE dll_t;

    dll_t dllopen(const char* dllname) {
        dll_t dll_ = LoadLibrary(dllname);
        if (dll_ == nullptr) {
            error::send_error(error::SystemError,
                language::error::dll_open_err);
        }
        return dll_;
    }

    void dllfree(dll_t name) {
        FreeLibrary(name);
    }

    void* dllfuncload(dll_t dll, const char* funcname) {
        void* func = GetProcAddress(dll, funcname);
        if (func == nullptr) {
            error::send_error(error::SystemError,
                language::error::dll_open_err);
        }
        return func;
    }

#elif defined(LINUX_PLAT)
    typedef void* dll_t;

    dll_t dllopen(const char* dllname) {
        dll_t dll_ = dlopen(dllname, RTLD_LAZY);
        char* err = dlerror();
        if (dll_ == nullptr || err != nullptr) {
            dlclose(dll_);
            error::send_error(error::SystemError,
                language::error::dll_open_err);
        }
        return dll_;
    }

    void dllfree(dll_t name) {
        dlclose(name);
    }

    void* dllfuncload(dll_t dll, const char* funcname) {
        void* func = dlsym(dll, funcname);
        if (func == nullptr) {
            error::send_error(error::SystemError,
                language::error::dll_open_err);
        }
        return func;
    }

#endif
}
}
