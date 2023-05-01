set_languages("c++20")

add_includedirs("third_party", "src/include")

target("Trc")
    set_kind("binary")
    add_files("src/**.cpp", "src/**.cppm")
    add_subdirs("language")
    add_deps("language")
