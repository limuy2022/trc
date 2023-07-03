/**
 * @file autodll.h
 * @brief 动态加载dll的接口
 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 */
module;
#include <cstring>
#include <language/error.hpp>
#include <string>
#ifdef __linux__
#include <filesystem>
#endif
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#endif
export module autodll;
import Error;

namespace trc::autodll {
// 定义加载出的动态链接库的储存类型
#ifdef _WIN32
typedef HINSTANCE dll_t;
#elif defined(__linux__)
typedef void* dll_t;
#endif

/**
 * @brief 打开一个动态链接库
 * @return dll
 */
export dll_t dllopen(const char*);

/**
 * @brief 关闭释放一个动态链接库
 */
export void dllfree(dll_t dll);

/**
 * @brief 从动态链接库中加载出一个函数
 * @param dll dll
 * @param funcname 函数名
 */
export void* dllfuncload(dll_t dll, const char* funcname);

/**
 * @brief 将链接库名字改为当前系统中的名字
 * @warning 不要加后缀和前缀
 * 另外记得用delete[]释放内存
 */
export char* redirect_to_platform(const std::string& name);

#ifdef _WIN32
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

#elif defined(__linux__)
namespace fs = std::filesystem;
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
