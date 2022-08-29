# 编译为dll才可以更换语言，所以无法编译到同一个文件中
add_library(Chinese SHARED
    chinese.cpp
)

add_library(English SHARED
    english.cpp
)

add_library(language SHARED
    english.cpp
)

target_compile_definitions(Chinese PRIVATE BUILD_language_DLL)
target_compile_definitions(English PRIVATE BUILD_language_DLL)
target_compile_definitions(language PRIVATE BUILD_language_DLL)
