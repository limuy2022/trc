set_languages("c++20")
add_includedirs("src/include")
add_includedirs("third_party")
add_includedirs("third_party/boost")

--去除那些烦人的警告
add_defines("_CRT_SECURE_NO_WARNINGS")

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
	unittest:add(find_library("gtest"))
	print("using gtest")
	if(is_os("linux")) then
		unittest:add("pthread")
	end
