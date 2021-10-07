cmake_minimum_required(VERSION 3.19)
project(share)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(LIBRARY_OUTPUT_PATH "../../bin/")

add_library(share SHARED
        ../utils/filesys.cpp
        ../utils/node.cpp
        ../utils/read.cpp
        ../utils/type.cpp
        ../Error/Error.cpp
        share.cpp
        compile_share.cpp
)
