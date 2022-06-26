file(GLOB_RECURSE SRCS "*.cpp")

if("${TRC_BUILD_TYPE}" STREQUAL "One")
add_library(Compiler STATIC
	${SRCS}
)
else()
add_library(Compiler SHARED
	${SRCS}
)
target_compile_definitions(Compiler PRIVATE BUILD_Compiler_DLL)
target_compile_definitions(Compiler PRIVATE shared_lib)
endif()

