file(GLOB_RECURSE SRCS "*.cpp")

if("${TRC_BUILD_TYPE}" STREQUAL "One")
add_library(base STATIC
    ${SRCS}
)
else()
add_library(base SHARED
    ${SRCS}
)
target_compile_definitions(base PRIVATE BUILD_base_DLL)
target_compile_definitions(base PRIVATE shared_lib)
endif()

