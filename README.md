## Trc是一个基于栈的编程语言

Trc是一个功能强大，工具链完善，容易上手的解释型语言，易于学习和使用，通过动态链接库的技术也容易被其他c++项目使用。

## 语言
完全由C++编写

## 目标

Python的简洁

Java的速度

lua的体积

## 项目站点
https://gitee.com/li-muyangangel/trc.git

本项目100%开源，所有源码都可以在gitee上找到

## 编译方式
本项目采用cmake编译程序，需要安装cmake并使用对应的工具链

注：本项目保证支持
msvc(vs系列,windows平台)，
gcc(linux)，
clang+llvm的编译方式，提供编译好的文件

当然啦，最好采用clang+llvm+msvc+windows的组合，这样出现错误的概率比较小，毕竟作者用的就是clang，其它编译器只是保证编译能够通过

如果采用的不是clang的编译方式，需要把CMakeLists.txt开头的
```
SET(CMAKE_C_COMPILER clang)
SET(CMAKE_CXX_COMPILER clang++)
```
给删去，否则编译器将始终为clang。对于linux用户，可能需要更改clang的路径

#### 注：要求是必须支持c++17及以上

## 更改语言（本项目一大特色）
本项目支持多种语言，支持方式是用相应的动态链接库文件覆盖掉原有的language文件即可

## 例子

### hello world
```
println("hello world!")
```

### 求1到n中的偶数
```
a := int(input())
while a > 0 {
    if a %2 == 0{
        println(a)
    }
    a = a- 1
}
```
### 大整数运算
```
a := 1231234561234
b := a
println(a)
println(b)
println(a + b)
c := a - b
println(c)
del "c"
println(a * b)
```
### 更多例子请参考
[更多例子](tests)

### 具体文档说明请参考
[索引](doc/INDEX.md)
