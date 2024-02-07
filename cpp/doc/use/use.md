# Trc使用说明

### 1.编写基础和注意事项

    1.双引号等同于单引号
    2.注释写法：#注释内容 or /*········*/
    3.为了便于传播，唯一支持的源文件编码是utf-8

### 2.变量

    1.新建变量：
    变量名 := 变量值
    例：var1 := 89

    2.删除变量
    删除变量：del
    例如：del "a"

    3.修改变量的值
    变量名 = 变量值
    例：var1 = 89
    
    注：该语言是一门既动态又静态的语言，例如：
    a := 90
    a = "pop"是错误行为，因为a的类型为int。但是，
    a:=90
    a:="pop"是正确行为，因为a的类型被修改了

### 3.语句

    1.导入模块：
    import '模块名'
    例：import 'module1'

    2.退出程序：
    exit(exit_code)

### 4.输入输出

    1.输出：
        1.print(值)
        例：print(12345, 90)
        2.println(值)
        例：println(12345)

    2.输入：
        2.1.input()获取输入并转换成字符串返回
        无参数:
        input()
        有参数:
        在获取输入前输出参数的值，类型不限
        例如:
        a:=input(1,'+', 1,"=")
        将输出1+1=

### 5.流程控制语句

    1.条件分支：
    if 条件表达式 {
        语句块
    } else if 条件表达式 {

    } else {

    }

    例如：
    if a == 1 {
        print(a)
    } else {

    }

    2.条件循环：


    while 条件表达式 {
        语句块
    } else {

    }
    

    3.function(函数)
    定义

    func xxx(a, b, n) {

    }
    
    调用：xxx()

    4.异常处理语句
    
    
    try {

    } catch error {

    } else {

    } finally {

    }
    

    5.goto
    例如：
    
    label:
    print(90)
    goto label

### 6.逻辑运算符

    1.==     相等
    2.!=     不相等
    3.<=     小于等于
    4.>=     大于等于
    5.<      小于
    6.>      大于
    7.and    与运算
    8.or     或运算
    9.not    非运算

### 7.运算符

    1.+      加
    2.-      减
    3.*      乘
    4./      除
    5.%      模运算
    6.**     乘方
    7.//     整除
    除法/是浮点型除法，需要整数除法请使用整除//

### 8.类型强制转换

    int(数据)
    string(数据)
    float(数据)

### 9.其它

    1.len(数据)
    获取数据长度
    2.help()
    获取帮助
    3.type()
    获取数据类型

### 10.调试功能

    1.断言
    assert [报错信息], 条件表达式
    如果为否，报出AssertError错误(报错信息为可选选项)

### 11.常量

    1.true,1
    2.false,0
    3.null,0

### 12.默认变量

    1.__name__:当前程序的模块名，主模块为__main__

### 13.类型系统

    1.int 整型
    2.float 浮点型
    3.long 长整型
    4.flong 长浮点型
    5.array 数组
    6.map 哈希表