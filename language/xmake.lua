-- 编译为dll才可以更换语言，所以无法编译到同一个文件中
target("Chinese")
    set_kind("shared")
    add_files("chinese.cpp", "./**.cppm")
    add_defines("BUILD_language_DLL")

target("English")
    set_kind("shared")
    add_files("english.cpp", "./**.cppm")
    add_defines("BUILD_language_DLL")

target("language")
    set_kind("shared")
    add_files("english.cpp", "./**.cppm")
    add_defines("BUILD_language_DLL")
