# Trc

## Trc is a stack-based programming language

Trc's syntax is easy,and have full toolchain. It is easy to learn modern c++ and compiler,too.

## language

100% C++

Using c++ 20 standard and use module feature

But here is a developing ```rust``` instance now!

[rust_trc](rust/)

## Goal

**as easy as Python**

**as fast as Java**

**as small as lua**

## project website

[Gitee](https://gitee.com/li-muyangangel/trc.git)
[GitHub](https://github.com/limuy2022/trc.git)

## All docs reference

[All docs](doc)

## How to compile

[Compilation guide](doc/developer/build.md)

## International

trc support many languages，the way to change language is that uses the language's dynamic linked library to replace the original language library

## Simple examples

### hello world

```go
println("hello world!")
```

### calculate even numbers from 1 to n

```go
a := int(input())
while a > 0 {
    if a %2 == 0{
        println(a)
    }
    a = a- 1
}
```

### Big num calculation

```go
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

[more example](tests/black_test/program)

## Contact author

Wechat:angelgel2020

QQ:3570249647

email: ```limuyang202011@163.com```

## Referenced Open-source software

| Library         |Usage     |
|:-----------|:-------|
| googletest | Unittest |

Express my thanks to Open-source software contributors!
