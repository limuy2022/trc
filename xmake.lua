set_languages("c++20")

add_includedirs("src/include")
add_requires("gtest")

add_rules("mode.debug", "mode.release")

target("Trc")
    set_kind("binary")
    add_files("src/**.cpp", "src/**.cppm")
    add_subdirs("language")
    add_deps("language")

target("unittest")
    set_kind("binary")
    add_files("src/**.cpp|Trc.cpp", "src/**.cppm", "tests/unittest/**.cpp", "tests/unittest/**.cppm")
    add_subdirs("language")
    add_deps("language")
    add_links("gtest_main")
    add_packages("gtest")
