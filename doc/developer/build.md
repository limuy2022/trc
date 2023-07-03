# 如何构建该项目

### 注：目前cmake因为模块支持的原因，暂时无法被用于编译该项目

### 注：要求编译器必须支持c++20标准，尤其是完整的模块支持，建议获取最新的编译器来参与编译

采用cmake或xmake编译程序，需要安装cmake或xmake

| 项目支持编译器(几乎支持了所有的主流编译器) |
|:-----------------------|
| msvc(vs)               |
| gcc(有mingw支持)          |
| clang                  |

| 项目支持平台 |
|:------------------|
| windows           |
| linux(有wsl2支持)    |
| macos(github action测试)|

注：如果编译器不满足上述要求仍然可以编译，
但操作系统不满足就会直接报错，无法编译

## 操作

注:由于该项目的linux和windows操作差不多，故没有加以区分

### 编译并安装发行版

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make install
```

或

```
xmake
xmake install
```

### 启动单元测试

```
mkdir unittest
cd unittest
cmake .. -DTRC_BUILD_TYPE=Test -DCMAKE_BUILD_TYPE=Release
make
../bin/unittest
```

或

```
xmake
xmake install
```
