/**
 * 跨平台需要使用的头文件
 */ 

#ifndef TRC_INCLUDE_PLATFORM_H
#define TRC_INCLUDE_PLATFORM_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define WINDOWS_PLAT
#elif defined(__linux__)
	#define LINUX_PLAT
#endif

// 字节序，true为大端，false为小端
extern const bool byte_order;

constexpr bool get_byte_order();

#endif
