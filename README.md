# Trc

## Trc是一个基于栈的编程语言

Trc是一个功能强大，工具链完善，容易上手的解释型语言，易于学习和使用，并且也容易被其他c++项目使用。

## 语言

完全由C++编写

## 目标

#### Python的简洁

#### Java的速度

#### lua的体积

## 项目站点

[Gitee](https://gitee.com/li-muyangangel/trc.git)
[GitHub](https://github.com/limuy2022/trc.git)

## 具体文档说明请参考

[代码文档](doc/html)
[所有文档](doc)

## 快速使用
#### 由于Linux系统兼容性过低，所以请参考下一章自行编译安装
#### windows请直接使用编译好的二进制文件或参考下一章的编译教程

## 如何编译此项目
[编译指南](doc/developer/build.md)

## 更改语言

trc支持多种语言，支持方式是用相应的动态链接库文件覆盖掉原有的language文件即可

## 简单例子

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

[更多例子](tests/black_test/program)

## 联系作者

微信:angelgel2020

QQ:3570249647

## 引用的开源软件
| 库名            | 用途     |
|:--------------|:-------|
| gflags        | 命令行解析库 |
| googletest    | 单元测试框架 |

在此向开源软件的作者表示感谢！
