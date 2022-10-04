/**
 * 英文的语句
 */

#include <clocale>
#include <cstdio>
#include <language/error.h>
#include <language/language.h>

namespace language {
namespace error {
    // 错误的信息具体定义，$标识待补充
    const char* error_map[] = { "NameError:", "ValueError:", "SyntaxError:",
        "VersionError:", "OpenFileError:", "ModuleNotFoundError:",
        "ArgumentError:", "ZeroDivError:", "RunError:", "AssertError:",
        "IndexError:", "MemoryError:", "KeyError:", "SystemError:",
        "OperatorError:", "RedefinedError:" };

    const char* error_from = "Error from ";
    const char* error_in_line = "Error in line ";

    // 格式化报错信息，规则：%代表插入的是字符串
    const char* nameerror = "name \"%\" is not defined.";
    const char* openfileerror = "Could't open \"%\".";
    const char* versionerror = R"(Could't run them.Because % is higher than %)";
    const char* memoryerror = "can't get the memory from os.";
    const char* zerodiverror = "\"%\" division by zero.";
    const char* modulenotfounderror = "Could't find \"%\" module.";
    const char* keyerror = "Key \"%\" is not defined.";
    const char* indexerror = R"(% is out of %)";
    const char* valueerror = R"("%" could not be "%")";
    const char* operatorerror = R"(Couldn't use % for types:% and %)";
    const char* funcredefinederror = "func % is redefined";
    const char* varredefinederror = "var % is redefined.";

    const char* argumenterror = R"(% need % arguments.)";

    const char* syntaxerror_int = R"(number % is incorrect.)";
    const char* syntaxerror_lexstring = R"(The string isn't end with " or ')";
    const char* syntaxerror_no_expect = R"(% is not be expected.)";
    const char* syntaxerror_expect = R"(% is excepted.)";
    const char* syntaxerror_lexanno = "Comments should end with */";
    const char* syntaxerror_escape_char
        = R"(Escape character % is not defined.)";
    const char* syntaxerror = "invalid syntax.";
    const char* syntaxerror_unmatched_char = "unmatched '%'";

    const char* asserterror_default = "assert";
    const char* asserterror_user = "%";

    const char* dll_open_err = R"(dll % was not found)";

    const char* noreach = "The program executed code that should "
                          "not have been executed.Please "
                          "report the problem to the Github "
                          "repository.";
    const char* magic_value_error
        = "Trc:\"%s\" is not a ctree file.Because its "
          "magic number is error\n";
}

namespace help {
    const char* help_msg = "TRC is a stack programming language. This "
                           "project implements most of the modern "
                           "programming language basics, provides a perfect "
                           "tool chain, which is suitable for working "
                           "scripts or embedded in your projects, and helps "
                           "to learn how to compile. ";
}

namespace TVM {
    const char* type_change_error_msg = "can't turn into ";
    const char* oper_not_def_error_msg = "can't use ";
}

namespace trc {
    const char* mode_not_found = " mode is not defined.\n";
}

namespace tdb {
    const char* var = "var";
    const char* not_defined = " is not defined.\n";
    const char* start_tip
        = R"(trc debugger is running.You can read 'Doc/use/TDB.txt' to find the help.\n)";
    const char* instruction = "instruction ";
}

void locale_init() {
    if (setlocale(LC_ALL, "en_US.UTF-8") == nullptr) {
        fprintf(stderr,
            "Can't load English language.Because there isn't English package "
            "installed on your computer\n");
    }
}
}
