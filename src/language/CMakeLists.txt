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
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    SET_TARGET_PROPERTIES (Chinese PROPERTIES COMPILE_FLAGS -Wno-invalid-source-encoding)
    SET_TARGET_PROPERTIES (English PROPERTIES COMPILE_FLAGS -Wno-invalid-source-encoding)
    SET_TARGET_PROPERTIES (language PROPERTIES COMPILE_FLAGS -Wno-invalid-source-encoding)
endif()
