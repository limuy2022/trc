cmake_minimum_required(VERSION 3.19)
project(TVM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(LIBRARY_OUTPUT_PATH "../../bin/")

add_library(TVM SHARED
        ../loader/code_loader.cpp
        ../loader/ctree_loader.cpp
        ../loader/func_loader.cpp
        obj/func.cpp
        TVM.cpp
        TRE.cpp
        ../memory/gc.cpp
        ../memory/new.cpp
        ../memory/free_TVM.cpp
        ../memory/memory_pool.cpp
        TVM_bytecode/TVM_condits.cpp
        TVM_bytecode/TVM_opers.cpp
        TVM_bytecode/TVM_var.cpp
        TVM_bytecode/TVM_others.cpp
        TVM_bytecode/TVM_func.cpp
        built_in/built_in_func.cpp
        types/trc_array.cpp
        types/trc_string.cpp
        types/trc_int.cpp
        types/trc_float.cpp
        types/bignum.cpp
        types/bigfloat.cpp
        types/base.cpp
)
