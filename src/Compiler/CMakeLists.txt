file(GLOB_RECURSE SRCS "*.cpp")
file(GLOB_RECURSE HEADERS "*../include/Compiler/*.h")

add_library(Compiler SHARED
	${SRCS}
	${HEADERS}
)

target_compile_definitions(Compiler PRIVATE BUILD_Compiler_DLL)
