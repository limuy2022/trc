add_subdirs(language)
add_subdirs(base)
add_subdirs(TVM)
add_subdirs(Compiler)
add_subdirs(trc)

if(is_os("linux")) then
    base:add("dl")
end
