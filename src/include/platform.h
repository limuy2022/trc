/**
 * 跨平台需要使用的头文件
 */

#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define WINDOWS_PLAT
#elif defined(__linux__)
	#define LINUX_PLAT
#endif

namespace trc {
	namespace utils {
		bool get_byte_order();
	}
	namespace def {
		// 字节序，true为大端，false为小端
		extern const bool byte_order;
	}
}
