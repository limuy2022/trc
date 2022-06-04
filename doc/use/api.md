# Trc提供的接口

| 动态链接库名称   | 接口位置                 | 功能                           |
|:----------|:---------------------|:-----------------------------|
| Compiler  | src/include/Compiler | 编译器，提供编译代码的函数，可以获得每一步的调用结果   |
| TVM       | src/include/TVM      | 运行字节码的虚拟机                    |
| base      | src/include/base     | 底层的基本实现，包含内存管理，异常处理，工具和日志等功能 |
| TVMbase   | src/include/TVMbase  | 虚拟机的底层部分，如类型和内存管理            |
| language等 | src/include/language | 语言插件，用来更换语言                  |
