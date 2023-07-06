# How to compile the project


### notice：require a compiler that is support c++ 20 standard,especially module support.Advice getting the newest compiler to compile.

 Use cmake or xmake to compile project，should install cmake or xmake

Network is required.Because this project uses xrepo to download packages.

| Support Compiler(almost suppout all the popular compilers) |
|:-----------------------------------------------------------|
| msvc(vs)                                                   |
| gcc(has mingw support)                                     |
| clang                                                      |

| support OS platforms      |
|:--------------------------|
| windows                   |
| linux(has wsl2 support)   |
| macos(github action test) |

Notice：if compiler don't support we can try compiling.But if OS don't support,we will stop compiling directly.

## what to do

Notice:This project compiling on different OS are similar

### Compile and install(include unittest)

```
xmake
xmake install
```
