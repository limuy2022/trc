file(GLOB_RECURSE SRCS "*.cpp")

add_library(Compiler SHARED
	${SRCS}
)

target_compile_definitions(Compiler PRIVATE BUILD_Compiler_DLL)
