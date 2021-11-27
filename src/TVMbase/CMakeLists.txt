set(LIBRARY_OUTPUT_PATH "../../bin")

file(GLOB_RECURSE SRCS "*.cpp")

add_library(TVMbase SHARED
	${SRCS}
)

target_compile_definitions(TVMbase PRIVATE BUILD_TVMbase_DLL)
