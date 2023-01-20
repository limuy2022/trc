/**
 * @file autodll.h
 * @brief 动态加载dll的接口
 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 */

#include <platform.hpp>
#include <string>

#ifdef WINDOWS_PLAT
#include <windows.h>
#elif defined(LINUX_PLAT)
#include <dlfcn.h>
#endif

namespace trc::autodll {
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
dll_t dllopen(const char*);

/**
 * @brief 关闭释放一个动态链接库
 */
void dllfree(dll_t dll);

/**
 * @brief 从动态链接库中加载出一个函数
 * @param dll dll
 * @param funcname 函数名
 */
void* dllfuncload(dll_t dll, const char* funcname);

/**
 * @brief 将链接库名字改为当前系统中的名字
 * @warning 不要加后缀和前缀
 * 另外记得用delete[]释放内存
 */
char* redirect_to_platform(const std::string& name);
}
