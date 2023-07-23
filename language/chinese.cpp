/**
 * 中文，作为dll插件提供不同语言
 */

module;
#include <clocale>
#include <cstdio>
module language;

namespace language {
namespace error {
    const char* error_map[] = { "名字错误:", "值错误:", "语法错误:",
        "版本错误:", "文件打开错误:", "模块找不到错误:", "参数错误:",
        "除零错误:", "运行错误:", "断言错误:", "索引错误:", "内存错误:",
        "键错误:", "系统错误:", "运算符错误:", "重定义错误:" };
    const char* error_from = "错误来自";
    const char* error_in_line = "错误发生在行";

    const char* nameerror = "名字\"%\"没有被定义.";
    const char* openfileerror = "无法打开?\"%\".";
    const char* versionerror
        = R"(无法执行它们，因为他们的版本号%比trc版本号%高)";
    const char* memoryerror = "无法从操作系统中申请内存.";
    const char* zerodiverror = "\"%\"被零除";
    const char* modulenotfounderror = "无法找到\"%\"模块.";
    const char* keyerror = "键\"%\"未定义";
    const char* indexerror = R"(%超出了%的范围)";
    const char* valueerror = R"("%"不能被转换为"%")";
    const char* operatorerror = R"(不能使用运算符"%"对于类型:% and %)";
    const char* funcredefinederror = "函数%被重定义";
    const char* varredefinederror = "变量%被重定义";

    const char* argumenterror = R"(%需要%个参数.)";

    const char* syntaxerror_int = R"(数字%不正确.)";
    const char* syntaxerror_lexstring = R"(这个字符串不以"或'结尾)";
    const char* syntaxerror_no_expect = R"(%是不被期待的)";
    const char* syntaxerror_expect = R"(%是被期待的.)";
    const char* syntaxerror_lexanno = "多行注释应当以*/结尾";
    const char* syntaxerror_escape_char = R"(转义符%未定义)";
    const char* syntaxerror = "错误的语法";
    const char* syntaxerror_unmatched_char = "未匹配的'%'";

    const char* asserterror_default = "断言";
    const char* asserterror_user = "%";

    const char* dll_open_err = "找不到dll\"%\"";

    const char* noreach = "这个项目运行了不应该被运行的代码"
                          ".请将这个问题报告给github仓库";
    const char* magic_value_error
        = "Trc:\"%s\"不是一个ctree文件.因为它的魔数不正确."
          "\n";
}
namespace help {
    const char* help_msg = "Trc是一门基于栈的编程语言。这个项目实现"
                           "了大部分现代编程语言的功能，提供了一个完"
                           "善的工具链。它很方便被嵌入到你的项目中或"
                           "者作为工作脚本，也可以帮助你去学习编译原理.";
}

namespace trc {
    const char* mode_not_found = "模式没有被定义\n";
}

namespace tdb {
    const char* var = "变量";
    const char* not_defined = "没有被定义";
    const char* start_tip = "trc的调试器正在运行.你可以阅读'Doc/"
                            "use/TDB.txt'以寻找帮助.\n";
    const char* instruction = "指令";
}

void locale_init() {
    if (setlocale(LC_ALL, "zh_CN.UTF-8") == nullptr) {
        fprintf(stderr,
            "Can't load Chinese language.Because there isn't Chinese package "
            "installed on your computer\n");
    }
}
}
