/**
 * 英文的语句
 */

#include "language/language.h"

namespace language {
    namespace error {
        // 错误的信息具体定义，$标识待补充
        const char * error_map[] = {
            "NameError:name \"$\" is not defined.",
            R"(ValueError:"$" could not be "$")",
            "TypeError:$",
            "SyntaxError:$",
            "VersionError:Could't run them.Because $ is higher than $",
            "OpenFileError:Could't open \"$\".",
            "ModuleNotFoundError:Could't find \"$\".",
            "ArgumentError:$.",
            "ZeroDivError:\"$\" division by zero.",
            "RunError:$",
            "AssertError:$",
            "IndexError:$ is out of $",
            "MemoryError:$",
            "KeyError:Key is not defined."
        };
        const char * error_from = "Error from ";
        const char * error_in_line = "Error in line ";
    }

    namespace help {
        const char * help_msg = "Trc is a programming language based on stack.This project from the\n\
most basic operators such as arithmetic, the branch structure, circulation structure, \n\
and then realize the constant folding, has realized the type of high precision operation, \n\
input and output as well as some commonly used built-in function, then realize the recycling, \n\
escaping strings, local and global variables,  This paper constructs a perfect error reporting \n\
system from compiler to interpreter, introduces the concept of module, makes the program can\n\
be modular programming, and establishes many practical tools, such as compile, run, decompile, \n\
batch compile, clean up files, code debugging, command line scripts and other functions\n";
    }

    namespace TVM {
        const char *type_change_error_msg = "can't turn into ";
        const char *oper_not_def_error_msg = "can't use ";
    }

    namespace trc {
        const char* mode_not_found = " mode is not defined.\n";
    }

    namespace tdb {
        const char * var = "var";
        const char* not_defined = " is not defined.\n";
        const char* start_tip = R"(trc debugger is running.You can read 'Doc/TDB.txt' to find the help.\n\n)";
        const char* instruction = "instruction ";
    }
}
