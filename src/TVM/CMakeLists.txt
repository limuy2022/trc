file(GLOB_RECURSE SRCS "*.cpp")
file(GLOB_RECURSE HEADERS "*../include/TVM/*.h")

add_library(TVM SHARED
	${SRCS}
	${HEADERS}
)

target_compile_definitions(TVM PRIVATE BUILD_TVM_DLL)
