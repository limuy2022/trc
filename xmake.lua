set_languages("c++20")
add_includedirs("src/include")
add_includedirs("third_party")
add_includedirs("third_party/boost")

--去除那些烦人的警告
add_defines("_CRT_SECURE_NO_WARNINGS")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_linkdirs("$(projectdir)/bin")

--分自己编写的代码和第三方库两部分构建，避免编译参数互相干扰
add_subdirs("src")
add_subdirs("third_party")

-- 将主程序编译为静态链接库，单元测试链接它执行
-- 添加单元测试
add_option("test")
	set_option_enable(false)
	set_option_showmenu(true)
	print("Test")
	add_subdirs("tests/unittest")
	find_library(gtest_path gtest)
	if(NOT gtest_path)
		message(FATAL_ERROR "can't find gtest.Maybe you can install googletest and add it to env path")
	endif()
	print("using gtest")
	target_link_libraries(unittest Trc ${gtest_path})
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		target_link_libraries(unittest pthread)
	endif()
endif()
