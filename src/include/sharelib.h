/**
 * 关于动态链接库的使用
 * 使用方法：
 * 该文件不是直接用来include的，而是一个模板
 * 为每个模块新建一个相应的dll.h
 * 将xxx替换成一个自定义的宏，然后在
 * 它的声明前加上TRC_xxx_api，并且在定义文件
 * 最上方加上“#define BUILD_xxx_DLL”或者在
 * cmake中添加target_compile_definitions（推荐）
 */

#pragma once

#include <platform.h>

#ifdef shared_lib
#ifdef WINDOWS_PLAT
#ifdef BUILD_xxx_DLL
#define TRC_xxx_api __declspec(dllexport)
#define TRC_xxx_func_api extern "C" __declspec(dllexport)
#else
#define TRC_xxx_api__declspec(dllimport)
#define TRC_xxx_func_api extern "C" __declspec(dllimport)
#endif
#else
#define TRC_xxx_api __attribute__((visibility("default")))
#define TRC_xxx_func_api extern "C" __attribute__((visibility("default")))
#endif
#else
#define TRC_xxx_api
#define TRC_xxx_func_api
#endif
