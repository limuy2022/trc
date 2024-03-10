# Trc programming language

Trc is a easy-learn programming language.It can be fast,safe and effective.

## Goal

**as easy as Python**

**as fast as Java**

**as small as lua**

## using language is 100% rust

As a modern language,rust provide us a lot of useful features and I enjoy myself in it.So this project is developed by pure rust.

## Build

Like other common rust project.Just use `cargo build`

Running tests is like other rust project,too.Just `cargo test`
But in order to read test data file,please run in the root dir.

## How to use

[Trc user Guide](docs/usage.md)

## Provide translation for this project

The project use rust-i18n to provide I18N(Internationalization) and L10N(localiation) support for the Trc.

You can do it following the docs about rust-i18n

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

## project website

[Gitee](https://gitee.com/li-muyangangel/trc.git)
[GitHub](https://github.com/limuy2022/trc.git)

## Contact author

Wechat:angelgel2020

QQ:3570249647

email: `limuyang202011@163.com`
