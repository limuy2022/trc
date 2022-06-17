file(GLOB_RECURSE SRCS "*.cpp")

add_library(TVM SHARED
	${SRCS}
)

target_compile_definitions(TVM PRIVATE BUILD_TVM_DLL)
