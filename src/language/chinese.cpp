/**
 * 中文的语句
 * 通过替换dll来达到更换语言的效果
 */
// 错误的信息具体定义，$标识待补充

#include "language/language.h"

namespace language {
    namespace error {
        const char * error_map[] = {
            "命名错误:名字\"$\"未被定义.",
            R"(值错误:"$"不能变成"$")",
            "类型错误:$",
            "语法错误:$",
            "版本错误:不能运行他们.因为$比$高",
            "打开文件错误:不能打开\"$\".",
            "模块未存在错误:不能找到\"$\".",
            "参数错误:$.",
            "除零错误:\"$\"被零除",
            "运行错误:$",
            "断言错误:$",
            "索引错误:$超出$",
            "内存错误:$",
            "键错误:键未定义."
        };
        const char * error_from = "错误来自";
        const char * error_in_line = "代码行";
    }
    namespace help {
        const char * help_msg = "Trc是一个基于栈的编程语言。本项目从最基础的运算\n\
符如四则运算，到分支结构，循环结构，然后实现了常量折叠，实现了关于高精度运算的类型，\n\
输入输出以及一些常用的内置函数，接着实现了垃圾回收，转义字符串，局部和全局变量，构建\n\
了从编译系统到解释器的完善报错系统，引入了模块的概念，使得程序可以进行模块化编程。\n\
建立了多个实际的工具，如编译，运行，反编译，批量编译，清理文件，代码调试，命令行脚本等功能";
    }

    namespace TVM {
        const char *type_change_error_msg = "不能转换成类型";
        const char *oper_not_def_error_msg = "不能使用";
    }

    namespace trc {
        const char* mode_not_found = " 模式没有被定义\n";
    }

    namespace tdb {
        const char * var = "变量";
        const char* not_defined = "未定义";
        const char* start_tip = "trc的调试器正在运行.你可以阅读'Doc/TDB.txt'去获取帮助.\n\n";
        const char* instruction = "指令";
    }
}
