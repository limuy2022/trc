file(GLOB_RECURSE SRCS "*.cpp")

add_executable(unittest
    ${SRCS}
)
target_include_directories(unittest PUBLIC "include/")