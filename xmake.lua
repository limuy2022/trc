set_project("Trc")
set_version("0.2")

set_languages("c++20", "c17")
set_targetdir("bin")

add_includedirs("src/include")
add_requires("gtest", {system=false})

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
add_rules("plugin.compile_commands.autoupdate")

target("Trc")
    set_kind("binary")
    add_files("src/**.cpp", "src/**.cppm")
    add_subdirs("language")
    add_deps("language")

target("unittest")
    set_kind("binary")
    set_default(false)
    add_defines("UNITTEST")
    add_files("src/**.cpp", "src/**.cppm", "tests/unittest/**.cpp", "tests/unittest/**.cppm")
    add_subdirs("language")
    add_deps("language")
    add_packages("gtest")
