cmake_minimum_required(VERSION 3.19)
project(Compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(LIBRARY_OUTPUT_PATH "../../bin/")

add_library(Compiler SHARED
        token.cpp
        grammar.cpp
        pre.cpp
        check.cpp
        optimize.cpp
        Compiler.cpp
        ../loader/code_loader.cpp
        ../loader/func_loader.cpp
        ../memory/free_compiler.cpp
        ../TVM/obj/func.cpp
)
