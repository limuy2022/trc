# 关于构建trc的提示

采用cmake编译程序，需要安装cmake

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

## cmake操作
注:由于该项目的linux和windows操作差不多，故没有加以区分
### 编译并安装发行版
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make install
```

### 启动单元测试

```
mkdir unittest
cd unittest
cmake .. -DTRC_BUILD_TYPE=Test -DCMAKE_BUILD_TYPE=Release
make
../bin/unittest
```

## 注：要求编译器必须支持c++20标准
