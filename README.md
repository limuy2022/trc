# Trc programming language

Trc is a easy-learn programming language. It can be fast,safe and effective.

| Platform | Status                                                                                                |
| :------- | :---------------------------------------------------------------------------------------------------- |
| Linux    | ![Linux Test](https://img.shields.io/github/actions/workflow/status/limuy2022/trc/rust_linux.yml)     |
| Winodows | ![Windows Test](https://img.shields.io/github/actions/workflow/status/limuy2022/trc/rust_windows.yml) |
| Macos    | ![Macos Test](https://img.shields.io/github/actions/workflow/status/limuy2022/trc/rust_macos.yml)     |

![Total Lines](https://tokei.rs/b1/github/limuy2022/trc)

## Goal

**as easy as Python**

**as fast as Java**

**as small as lua**

## using language is 100% rust

As a modern language,rust provide us a lot of useful features and I enjoy myself in it.So this project is developed by pure rust.

## Build

Like other common rust project.Just use `cargo build --all`

Running tests is like other rust project,too.Just `cargo test --all`
But in order to read test data file,please run in the root dir.

## How to use

[Trc Programming Book](docs/usage.md)

[Trc Programming Book中文版](docs/usage-zh.md)

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
        println("{}", a)
    }
    a = a- 1
}
```

### fast pow

```go
func fastpow(a: int, b: int) int {
  if b == 0 {
    return 1
  }
  tmp := fastpow(a, b // 2)
  tmp = tmp * tmp
  if b % 2 != 0 {
   tmp = tmp * a
  }
  return tmp
}

print("{}", fastpow(2, 3))
```

## project website

[Gitee](https://gitee.com/li-muyangangel/trc.git)
[GitHub](https://github.com/limuy2022/trc.git)

## Contact author

Wechat:angelgel2020

QQ:3570249647

email: `limuyang202011@163.com`
