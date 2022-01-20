/**
 * 中文的语句
 * 通过替换dll来达到更换语言的效果
 * 注意：该文件的编码是特殊的，为了适应中文，编码为GBK
 */

// 错误的信息具体定义，$标识待补充

#include "language/error.h"
#include "language/language.h"

namespace language {
namespace error {
    const char* error_map[] = { "命名错误:", "值错误:",
        "语法错误:", "版本错误:", "打开文件错误:",
        "模块未存在错误:", "参数错误:", "除零错误:",
        "运行错误:", "断言错误:", "索引错误:", "内存错误:",
        "键错误:", "系统错误:" };
    const char* error_from = "错误来自";
    const char* error_in_line = "代码行";

    const char* nameerror = "名称\"%\"未定义.";
    const char* openfileerror = "不能打开\"%\".";
    const char* versionerror = R"(不能运行它们.因为%比%高)";
    const char* memoryerror = "不能从操作系统申请内存.";
    const char* zerodiverror = "\"%\"被零除";
    const char* modulenotfounderror = "不能找到模块\"%\".";
    const char* keyerror = "键\"%\"未定义";
    const char* indexerror = R"(%超出了%的范围)";
    const char* valueerror = R"("$"不能变成"$")";

    const char* argumenterror = R"(%需要%个参数.)";

    const char* syntaxerror_int = R"(数字%是不正确的.)";
    const char* syntaxerror_lexstring
        = R"(这个字符串应当以"或'结尾)";
    const char* syntaxerror_no_expect
        = R"(%不是被期待的字符)";
    const char* syntaxerror_expect = R"(%是被期待的字符.)";
    const char* syntaxerror_lexanno = "注释应该以*/结尾";

    const char* asserterror_default = "断言";
    const char* asserterror_user = "%";

    const char* dll_open_err = "动态链接库\"%\"加载失败";

    const char* noreach
        = "程序执行了不应该执行的代码，请将该问题"
          "反馈给github仓库";
    const char* magic_value_error
        = "Trc:\"%s\"不是一个ctree文件.因为它的魔数是错误的"
          "\n";
}
namespace help {
    const char* help_msg
        = "Trc是一个基于栈的编程语言。本项目从最基础的运算\n\
符如四则运算，到分支结构，循环结构，然后实现了常量折叠，实现了关于高精度运算的类型，\n\
输入输出以及一些常用的内置函数，接着实现了垃圾回收，转义字符串，局部和全局变量，构建\n\
了从编译系统到解释器的完善报错系统，引入了模块的概念，使得程序可以进行模块化编程。\n\
建立了多个实际的工具，如编译，运行，反编译，批量编译，清理文件，代码调试，命令行脚本等功能";
}

namespace TVM {
    const char* type_change_error_msg = "不能转换成类型";
    const char* oper_not_def_error_msg = "不能使用";
}

namespace trc {
    const char* mode_not_found = " 模式没有被定义\n";
}

namespace tdb {
    const char* var = "变量";
    const char* not_defined = "未定义";
    const char* start_tip
        = "trc的调试器正在运行.你可以阅读'Doc/"
          "TDB.txt'去获取帮助.\n";
    const char* instruction = "指令";
}
}
