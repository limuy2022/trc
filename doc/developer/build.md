# 关于构建trc的提示

采用cmake或xmake编译程序，需要安装cmake或xmake

| 项目支持编译器(几乎支持了所有的主流编译器) |
|:-----------------------|
| msvc(vs)               |
| gcc(有mingw支持)          |
| clang                  |

| 项目支持平台(没有macOS支持) |
|:------------------|
| windows           |
| linux(wsl2测试)     |

注：如果编译器不满足上述要求仍然可以编译，
但操作系统不满足就会直接报错，无法编译

## cmake操作:
### 编译debug需要添加的参数

```
-DCMAKE_BUILD_TYPE=Debug
```

### 编译release需要添加的参数

```
-DCMAKE_BUILD_TYPE=Release
```

### 启动单元测试需要的参数(不加就是默认不启动)

```
-DTRC_BUILD_TYPE=Test
```

### 不启动单元测试需要的参数(显式指定不编译单元测试)

```
-DTRC_BUILD_TYPE=Run
```

## xmake操作:

## 注：要求编译器必须支持c++20标准