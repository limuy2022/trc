set_project("Trc")
set_version("0.2")

set_languages("c++23", "c17")
set_targetdir("bin")

add_requires("gtest")
-- 启用所有警告，并且作为编译错误处理
set_warnings("all", "error")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
add_rules("plugin.compile_commands.autoupdate")
add_cxxflags("-Wno-read-modules-implicitly", "-Wno-unused-but-set-variable")
add_includedirs("src/compiler")
add_rules("lex", "yacc")

target("Trc")
    set_kind("binary")
    -- on_load(function (target)
    --     -- 用于生成bison和flex的文件，该跨平台问题并不显著，因为bison和flex名称接口和行为是固定的
    --     os.run("flex -o src/compiler/token.cpp src/compiler/token.ll")
    --     os.run("bison -o src/compiler/parser.cpp src/compiler/parser.yy")
    -- end)
    add_files("src/**.cpp", "src/**.cppm", "language/**.cppm")
    add_files("src/**.ll", "src/**.yy")
    includes("language")
    add_deps("language")

target("unittest")
    set_kind("binary")
    set_default(false)
    add_defines("UNITTEST")
    add_files("src/**.cpp", "src/**.cppm", "tests/unittest/**.cpp", "tests/unittest/**.cppm")
    includes("language")
    add_deps("language")
    add_packages("gtest")
