cmake_minimum_required(VERSION 3.16)
SET(CMAKE_C_COMPILER clang)
SET(CMAKE_CXX_COMPILER clang++)
project(Trc)

include_directories(src/include)
link_directories(bin)

set(rele_comp "-O3 -fno-rtti -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -std=c++17 -fdata-sections -ffunction-sections -fvisibility=hidden")
set(debug_comp "-g -O0 -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-exceptions -std=c++17 -fdata-sections -ffunction-sections -fvisibility=hidden")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    #release
    set(CMAKE_CXX_FLAGS ${rele_comp})
    #debug
    #set(CMAKE_CXX_FLAGS ${debug_comp})
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    #release
    set(CMAKE_CXX_FLAGS ${rele_comp})
    #debug
    #set(CMAKE_CXX_FLAGS ${debug_comp})
else()
    set(CMAKE_CXX_FLAGS "/nologo /std:c++latest")
    #release
    set(CMAKE_BUILD_type "release")
    #debug
    #set(CMAKE_BUILD_type "debug")
endif()

add_subdirectory(src/language)
add_subdirectory(src/base)
add_subdirectory(src/TVM)
add_subdirectory(src/Compiler)
add_subdirectory(src/trc)
add_subdirectory(src/TVMbase)

target_link_libraries(base language)
target_link_libraries(TVMbase base language)
target_link_libraries(TVM language base TVMbase Compiler)
target_link_libraries(Compiler language base TVMbase)
target_link_libraries(Trc language base TVM Compiler)
