cmake_minimum_required(VERSION 3.19)
project(Trc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(EXECUTABLE_OUTPUT_PATH "bin/")

add_subdirectory(src/Compiler)
add_subdirectory(src/TVM)
add_subdirectory(src/share)

aux_source_directory(src/tools DIR_SRCS)

add_executable(Trc
        src/Trc.cpp
        src/Trc_logo.rc
        ${DIR_SRCS}
)

link_directories("bin/")
target_link_libraries(Compiler share)
target_link_libraries(TVM share)
target_link_libraries(TVM Compiler)
target_link_libraries(Trc TVM)
target_link_libraries(Trc Compiler)
