/**
 * @file autodll.cpp
 * @brief 显式加载动态链接库的包装
 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 */

#include <base/Error.h>
#include <base/autodll.h>
#include <cstring>
#include <filesystem>
#include <language/error.h>
#include <platform.h>
#include <string>

namespace fs = std::filesystem;

namespace trc::autodll {
// 定义加载出的动态链接库的储存类型
#ifdef WINDOWS_PLAT
dll_t dllopen(const char* dllname) {
    dll_t dll_ = LoadLibrary(dllname);
    if (dll_ == nullptr) {
        error::send_error(
            error::SystemError, language::error::dll_open_err, dllname);
    }
    return dll_;
}

void dllfree(dll_t name) {
    FreeLibrary(name);
}

void* dllfuncload(dll_t dll, const char* funcname) {
    void* func = (void*)GetProcAddress(dll, funcname);
    if (func == nullptr) {
        error::send_error(error::SystemError, language::error::dll_open_err);
    }
    return func;
}

char* redirect_to_platform(const std::string& name) {
    size_t n = name.length();
    char* res = new char[n + 1 + 4];
    memcpy(res, name.c_str(), n);
    strcpy(res + n, ".dll");
    return res;
}

#elif defined(LINUX_PLAT)
dll_t dllopen(const char* dllname) {
    dll_t dll_ = dlopen(dllname, RTLD_LAZY);
    char* err = dlerror();
    if (dll_ == nullptr || err != nullptr) {
        error::send_error(
            error::SystemError, language::error::dll_open_err, dllname);
    }
    return dll_;
}

void dllfree(dll_t name) {
    dlclose(name);
}

void* dllfuncload(dll_t dll, const char* funcname) {
    void* func = dlsym(dll, funcname);
    if (func == nullptr) {
        error::send_error(error::SystemError, language::error::dll_open_err);
    }
    return func;
}

char* redirect_to_platform(const std::string& name) {
    size_t n = name.length();
    char* res = new char[n + 3 + 1 + 2];
    auto name_path = fs::path(name);
    const std::string& parent_path = name_path.parent_path().string();
    strcpy(res, parent_path.c_str());
    res[parent_path.length()] = '/';

    strcpy(res + parent_path.length() + 1, "lib");

    const std::string& filename_path = name_path.filename().string();
    strcpy(res + parent_path.length() + 1 + 3, filename_path.c_str());

    strcpy(res + parent_path.length() + 1 + 3 + filename_path.length(), ".so");
    return res;
}

#endif
}
