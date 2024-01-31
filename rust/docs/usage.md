# The usage of Rust version Trc

## Hello world

Writing a hello world is the beginning of learning Trc.

```rust
println("Hello World!")
```

Great!Now you are saying "Hello" to the programming world!

## First written value for Trc

you can write integer or float value in the following ways:
|Way|Explain|
|:---|:---|
|12321312|commom value|
|122_32432_4324324|use underline to split the number to read more easily|
|0b32132|number in 2 radix|
|0o324243|number in 8 radix|
|0x324324|number in 16 radix|
|1e9+7 or 2E9+7|Scientific notation|

Tip1:No matter how large is your number.Trc compiler will take it into enough type to store!Don't worry!

Tip2:The form like 0x3.4 isn't supported

Tip3:The form like 001 is supported

The next is the string value. To make you to write strings more easily:

|Way|Explain|
|:---|:---|
|"hello world"|commom value|
|'hello_world'|another method that equals to "hello world"|
|"""hello world"""|this method is for code across multiple lines|

There are also many easape char in the string:

|escape char|meaning|
|:---|:---|
|\t|tab|
|\n|new line|
|\\\\|\|
|\'|'|
|\"|"|
|\0|the tick of the end of the string|

If you add ```r``` or ```R``` in front of the string.Trc will treat it as a raw string.
Yes.These rules are from Python.I love its grammar rules

ok.now you know how to write value in Trc.Then let's go to the next part.

## The operators of trc

the operators for basic types of trc is like others language.

Here are the operator support

|Operator|Explain|
|:---|:---|
|+|addition|
|-|subtraction|
|*|multiplication|
|/|division|
|//|divisible|
|%|mod|
|**|exponent|
|<|less than|
|>|greater than|
|<=|less than or equal to|
|>=|greater than or equal to|
|==|equal to|
|!=|not equal to|
|&&|and|
|\|\||or|
|\||bit or|
|&|bit and|
|^|bit xor|
|~|bit not|
|<<|bit left shift|
|>>|bit right shift|
|!|not|

Obviously,operators like ```+=``` is supported,too.

But,something should be noticed is that you cannot use logical operators for ```int``` or anything else,just for bool.

So,code like this cannot be compiled successfully:

```rust
a := 1
if a {
    println(a)
}
```

Ok,just like others language,but there is an important difference.
you cannot use the different types of values to calaulate

For example:

```go
a:=9
b:=7.8
a+b
```

the compiler will report an error

## the control flow of Trc

First,you can use ```if ... else if ... else``` statements

An example:

```python
if 1 == 1 {

} else if 2 != 1 {

} else {

}
```

```for``` support two kinds

First:

```cpp
for i := 0; i < 10; i++ {
    println(i)
}
```

Second:

```python
for i in xxx {
    println(i)
}
```

```xxx``` should be a iterable var

## the comments of Trc

Trc support two kinds of comments

the first is use ```#```,from ```#``` to the end of the line belongs to the comment

the second is use ```/**/```,this kind can cross the line,like:

```cpp
/*
hello world!
*/
```

## the var of trc

First,we support the UTF-8 with your var name.So you can define your var like this:

```go
你好:=90
```

the compiler will regard this var as a int var.

Sometimes maybe you want to define the type.Do it like this:

```go
int a:=90
```

## Data structures for Trc

Std lib provide many kinds of data structures for Trc.Here is the list:

|Structure|
|:---|
|St table|
|suffix automaton|
|ac automaton|
|list|
|forward list|
|stack|
|deque|

## Function

Define a function like this:

```go
func add(int a, int b) -> int {
    return a + b;
}
```

Or define a template function:

```go
func add<T>(T a, T b) -> T {
    return a + b;
}
```
