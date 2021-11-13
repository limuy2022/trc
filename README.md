### Trc是一个基于栈的编程语言

本项目从最基础的运算符如四则运算，到分支结构，循环结构，然后实现了常量折叠，实现了关于高精度运算的类型，输入输出以及一些常用的内置函数，接着实现了垃圾回收，转义字符串，局部和全局变量，构建了从编译系统到解释器的完善报错系统，引入了模块的概念，使得程序可以进行模块化编程

建立了多个实际的工具，如编译，运行，反编译，批量编译，清理文件，代码调试，命令行脚本等功能

## 语言
完全由C++编写

## 目标

Python的简洁

Java的速度

lua的体积

## 项目站点
https://gitee.com/li-muyangangel/trc.git

本项目100%开源，所有源码都可以在gitee上找到

## 系统要求
编译器：g++8.1.0是最好的，但并不是强制要求,在其它编译器上结果可能有所不同

注：同样支持VS2019

操作系统：任意

## 使用方式
对于绝大部分功能只需要把bin目录拷走就可以执行了
而对于完善的功能，则需要bin和doc以及标准库文件，最好把源代码也带走

## 编译方式
本项目采用cmake编译程序，需要安装cmake并使用对应的工具链
buildtrc(一个帮助编译trc的工具)
windows：

    有安装mingw：
    buildtrc gcc
    build
    有安装vs：
    buildtrc vs
    build
    有安装vs且使用nmake进行编译:（不要在普通的命令行下运行，要在vs提供的命令行下运行)
    buildtrc msvc
    build

注：这是buildtrc绝对支持的方式，但用户也可以自由选择其他工具链进行编译
方法如下：

    cmake . -G "对应名称"
    对应make工具（如make,nmake)
其它操作系统:

    Linux:可以跨平台
    Mac:未知

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
[索引](INDEX.txt)

## 目录功能说明
### doc:帮助与说明文档
### tests:测试用例，也是学习的好样例
### bin:编译后的可执行文件
### src/Compiler:编译器
### src/Error:异常处理系统
### src/include:头文件
### src/loader:转换器
### src/memory:内存管理
### src/share:多个模块共享的数据和函数
### src/tools:命令行工具
### src/TVM:执行字节码的虚拟机
### src/utils:开发过程中的工具函数存放模块

注：编译器采用面向过程的思想，而虚拟机采用面向对象的思想
