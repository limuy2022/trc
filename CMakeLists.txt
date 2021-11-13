cmake_minimum_required(VERSION 3.16.3)
project(Trc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH "bin/")
INCLUDE_DIRECTORIES(src/include)

add_executable(Trc
    src/Trc.cpp
    src/Compiler/check.cpp
    src/Compiler/Compiler.cpp
    src/Compiler/optimize.cpp
    src/Compiler/pre.cpp
    src/Compiler/token.cpp
    src/Compiler/grammar/grammar.cpp
    src/Compiler/grammar/obj.cpp
    src/Error/Error.cpp
    src/loader/code_loader.cpp
    src/loader/ctree_loader.cpp
    src/loader/func_loader.cpp
    src/memory/free_compiler.cpp
    src/memory/free_TVM.cpp
    src/memory/memory_pool.cpp
    src/memory/new.cpp
    src/memory/objs_pool_TVM.cpp
    src/share/compile_share.cpp
    src/share/share.cpp
    src/tools/all.cpp
    src/tools/brun.cpp
    src/tools/build.cpp
    src/tools/clean.cpp
    src/tools/crun.cpp
    src/tools/dis.cpp
    src/tools/grammar.cpp
    src/tools/run.cpp
    src/tools/tdb.cpp
    src/tools/token.cpp
    src/tools/tshell.cpp
    src/TVM/TRE.cpp
    src/TVM/TVM.cpp
    src/TVM/built_in/built_in_func.cpp
    src/TVM/obj/func.cpp
    src/TVM/packages/lib.cpp
    src/TVM/packages/importlib/math.cpp
    src/TVM/TVM_bytecode/TVM_condits.cpp
    src/TVM/TVM_bytecode/TVM_func.cpp
    src/TVM/TVM_bytecode/TVM_opers.cpp
    src/TVM/TVM_bytecode/TVM_others.cpp
    src/TVM/TVM_bytecode/TVM_var.cpp
    src/TVM/types/base.cpp
    src/TVM/types/trc_array.cpp
    src/TVM/types/trc_float.cpp
    src/TVM/types/trc_flong.cpp
    src/TVM/types/trc_int.cpp
    src/TVM/types/trc_long.cpp
    src/TVM/types/trc_map.cpp
    src/TVM/types/trc_string.cpp
    src/utils/filesys.cpp
    src/utils/node.cpp
    src/utils/read.cpp
    src/utils/type.cpp
    src/Trc_logo.rc
)