# Trc programming language

Trc is a easy-learn programming language.It can be fast,safe and effective.

## Goal

**as easy as Python**

**as fast as Java**

**as small as lua**

there are two ways to use it:

[c++ version](./cpp/)
[rust version](./rust/)

They have their own adventages.You can choose by your preference.

The c++ version is the first version of trc.But after I uograded it to cpp 20 stardand.it bacame hard to compile and develop.And there is a lot of trouble codes and bugs in it.So I want to stop to develop it until the cpp 20 standard is more stable.

The rust version is the second version of trc.It is under development now.It is designed better.

I don't know which will be the main version.Maybe both?

## International

Trc supports many different kinds of languages.We use GNU gettext.So it dont't need change the language by hand.

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

email: ```limuyang202011@163.com```
