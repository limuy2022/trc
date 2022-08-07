cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

# ccache的使用
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
	message("using ccache")
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

include_directories(src/include)
include_directories(third_party)

#去除那些烦人的警告
add_definitions(-D _CRT_SECURE_NO_WARNINGS)

project(Trc CXX)

message("${CMAKE_BUILD_TYPE}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

link_directories(${PROJECT_SOURCE_DIR}/bin)

# 分自己编写的代码和第三方库两部分构建，避免编译参数互相干扰
add_subdirectory(third_party)
add_subdirectory(src)

message("${TRC_BUILD_TYPE}")
# 将主程序编译为静态链接库，单元测试链接它执行
# 添加单元测试
if("${TRC_BUILD_TYPE}" STREQUAL "Test")
	add_subdirectory(tests/unittest)
	find_library(gtest_path gtest)
	if(NOT gtest_path)
		message(FATAL_ERROR "can't find gtest.Maybe you can install googletest and add it to env path")
	endif()
	message("using gtest")
	target_link_libraries(unittest Trc ${gtest_path})
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		target_link_libraries(unittest pthread)
	endif()
endif()
