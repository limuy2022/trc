file(GLOB_RECURSE SRCS "*.cpp")

# 去掉特殊的非测试文件
list(REMOVE_ITEM SRCS "create_dll.cpp")

add_executable(unittest
    ${SRCS}
)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/tests/unittest/testdata/autodll)
add_library(create_dll SHARED
        testdata/autodll/create_dll.cpp
        )
target_include_directories(unittest PUBLIC "include/")
