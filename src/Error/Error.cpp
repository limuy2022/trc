#include <iostream>
#include <string>
#include <cstdarg>
#include "../include/Error.h"
#include "../include/share.h"

using namespace std;

namespace error_env{
    /**
     * 报错设置
     * 系统需要知道当前处于什么模式，以合适的模式应对发生的状况
     */ 
    // 是否终止程序
    bool quit = true;
    // 是否显示出错误信息，如果为否将会把错误信息存入栈中
    bool out_msg = true;
}

// 错误map，<错误名称, 错误模板字符串>
static const string error_map[] = {
        "NameError:name \"$\" is not defined.",
        "ValueError:\"$\" could not be \"$\"",
        "TypeError:$",
        "SyntaxError:$",
        "VersionError:Tree could't run them.Because $ is higher than $",
        "OpenFileError:could't open \"$\".",
        "ModuleNotFoundError:Tree could't find \"$\".",
        "ArgumentError:$.",
        "ZeroDivError:\"$\" division by zero.",
        "RunError:$",
        "AssertError:$",
        "IndexError:$ is out of $",
        "MemoryError:$",
        "KeyError:key is not defined."
};

string make_error_msg(const int error_name, va_list &ap) {
    /**
     * 通用函数，填充报错信息
     * error_name：异常名
     * ap：可变参数
     */

    string finally_out = error_map[error_name];
    string::size_type index;
    for(;;) {     
        index = finally_out.find("$");
        if(index == string::npos) return finally_out;
        finally_out.replace(index, 1 , va_arg(ap, const char*)); 
    }
}

void send_error(const int name, ...) {
    /**
     * 运行时报出错误，错误名称和错误信息
     * name:错误名
     * 可变参数：const char* 类型的字符串
     */

    va_list ap;
    va_start(ap, &name);
    send_error_(make_error_msg(name, ap));
    va_end(ap);
    if(error_env::quit) exit(1);
}

void error_type(const int name, const string &err_msg, int err_value_type) {
    /**
     * 该函数用于在err_msg后链接报错类型，将其转化为字符串，链接后报错
     * 例如:error_type(TypeError, "func xxx can't be used for ", 1)
     * 报错:
     * .....
     * TypeError:func xxx can't be used for int
     * 注：仅可以报出填充数量为一的异常
     */

    send_error(name, (err_msg + type_int::int_name_s[err_value_type]).c_str());
}

void send_error_(const string &error_msg) noexcept{
    /**
     * 不接收可变参数，接受已经处理好的报错信息
     */
    
    cerr << "\n" << "Error from " << run_env::run_module << "\n" \
    << "Error in line " << LINE_NOW + 1 << ":\n"\
    << error_msg << "\n";
}
