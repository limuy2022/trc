file(GLOB_RECURSE SRCS "*.cpp")

if("${TRC_BUILD_TYPE}" STREQUAL "Test")
    add_library(Trc STATIC
        ${SRCS}
    )
else()
    add_executable(Trc
        ${SRCS}
        Trc_logo.rc
    )
endif()
