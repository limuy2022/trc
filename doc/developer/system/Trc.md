# Trc系统架构

### 程序入口：trc.exe

### TVM底层交互：TVMbase.dll

### TVM:TVM.dll

### 编译器:Compiler.dll

### 底层基础：base.dll

### 语言模块:language.dll等

### 具体关系


-----------------------------------

             trc.exe
-----------------------------------
     Compiler.dll | TVM.dll
-----------------------------------
            TVMbase.dll
-----------------------------------
             base.dll
-----------------------------------
        langauge.dll
-----------------------------------

### 依赖关系