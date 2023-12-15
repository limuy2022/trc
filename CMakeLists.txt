cmake_minimum_required(VERSION 3.28)
project(Trc CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
link_directories("${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

add_subdirectory("cmake_third_party/googletest")
include_directories("cmake_third_party/googletest/googletest/include", "include")

add_library(language_module)
target_sources(language_module
    PUBLIC
    FILE_SET CXX_MODULES FILES
    language/language.cppm
)

message("${LANGUAGE_SOURCES}")

add_library(Chinese language/chinese.cpp)

add_library(English language/english.cpp)

add_library(language language/english.cpp)

target_link_libraries(Chinese language_module)
target_link_libraries(English language_module)
target_link_libraries(language language_module)

file(GLOB_RECURSE CPPMSOURCES "*.cppm")
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")

message(${CPP_SOURCES})

add_library(foo)

target_sources(foo
    PUBLIC
    FILE_SET CXX_MODULES FILES
    ${CPPMSOURCES}
)

add_executable(Trc ${CPP_SOURCES})

target_link_libraries(Trc foo)
target_link_libraries(Trc language)


file(GLOB_RECURSE TEST_SOURCES "tests/unittest/*.cpp")
file(GLOB_RECURSE TEST_MODULE_SOURCES "tests/unittest/*.cppm")
add_library(testmodulefiles)

target_sources(testmodulefiles
    PUBLIC
    FILE_SET CXX_MODULES FILES
    ${CPPMSOURCES}
    ${TEST_MODULE_SOURCES}
)

add_executable(unittest
   ${CPP_SOURCES}
   ${TEST_SOURCES}
)
target_compile_definitions(unittest PRIVATE "UNITTEST")
target_link_libraries(unittest testmodulefiles language gtest)
add_dependencies(Trc flex bison)
add_dependencies(unittest flex bison)

add_custom_target(flex
    command flex -o ${PROJECT_SOURCE_DIR}/src/compiler/token.cpp ${PROJECT_SOURCE_DIR}/src/compiler/token.ll
)
add_custom_target(bison
    command bison -o ${PROJECT_SOURCE_DIR}/src/compiler/parser.cpp ${PROJECT_SOURCE_DIR}/src/compiler/parser.yy
)
