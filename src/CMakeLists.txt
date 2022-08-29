set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(compile_flag " -Wextra -Wall -fdata-sections -ffunction-sections -fno-exceptions")
if(NOT "${TRC_BUILD_TYPE}" STREQUAL "Test")
	set(compile_flag " ${compile_flag} -fno-rtti")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${compile_flag}")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 ${compile_flag}")
	set(CMAKE_CXX_FLAGS_MinSizeRel "${CMAKE_CXX_FLAGS_MinSizeRel} ${compile_flag}")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${compile_flag}")
	if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl")
		list(APPEND CMAKE_SHARED_LINKER_FLAGS "-Wl")
	endif()
endif()

file(GLOB_RECURSE SRCS "*.cpp")

if("${TRC_BUILD_TYPE}" STREQUAL "Test")
	add_library(Trc STATIC
			${SRCS}
			)
else()
	add_executable(Trc
			${SRCS}
			trc/Trc_logo.rc
			)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(Trc dl)
endif()

target_link_libraries(Trc language gflags::gflags easyloggingpp)
