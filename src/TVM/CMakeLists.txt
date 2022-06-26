file(GLOB_RECURSE SRCS "*.cpp")

if("${TRC_BUILD_TYPE}" STREQUAL "One")
add_library(TVM STATIC
	${SRCS}
)
else()
add_library(TVM SHARED
	${SRCS}
)
target_compile_definitions(TVM PRIVATE BUILD_TVM_DLL)
target_compile_definitions(TVM PRIVATE shared_lib)
endif()

