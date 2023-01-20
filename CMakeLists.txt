cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	# release模式下开启LTO的支持
	message("LTO is opened")
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# ccache的使用
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
	message("using ccache")
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

include_directories(src/include)
include_directories(third_party)

#去除msvc编译器的不安全警告
add_definitions(-D _CRT_SECURE_NO_WARNINGS)

project(Trc CXX)

message("${CMAKE_BUILD_TYPE}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

link_directories(${PROJECT_SOURCE_DIR}/bin)

message("${TRC_BUILD_TYPE}")
# 将主程序编译为静态链接库，单元测试链接它执行
# 添加单元测试
if("${TRC_BUILD_TYPE}" STREQUAL "Test")
	add_definitions(-D UNITTEST)
	add_subdirectory(tests/unittest)
	add_subdirectory(third_party/googletest)
	include_directories(third_party/googletest/googletest/include)
	message("using gtest")
	target_link_libraries(unittest Trc gtest)
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		target_link_libraries(unittest pthread)
	endif()
endif()
# 分自己编写的代码和第三方库两部分构建，避免编译参数互相干扰
add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(language)

install(TARGETS Trc language)
