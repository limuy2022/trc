set(EXECUTABLE_OUTPUT_PATH "../../bin/")
file(GLOB_RECURSE SRCS "*.cpp")

add_executable(Trc
    ${SRCS}
    Trc_logo.rc
)
