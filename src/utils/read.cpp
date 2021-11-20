#include <string>
#include <cstdio>
#include <cstdarg>
#include "Error.h"

using namespace std;

void readcode(string &file_data, const string &path) {
    /*
    * 读取文件并返回字符串
    */

    file_data.clear();
    FILE *file = fopen(path.c_str(), "r");
    if (file == nullptr)
        send_error(OpenFileError, path.c_str());
    char tmp = fgetc(file);
    while (!feof(file)) {
        file_data += tmp;
        tmp = fgetc(file);
    }
    fclose(file);
}

void readfile(string &file_data, const string &path, const int err, ...) {
    /**
     * 与上面那种方法不同在于可以自由选择报错类型，但失败同样会结束程序
     * err：错误名
     */

    file_data.clear();
    va_list ap;
    va_start(ap, &err);
    FILE *file = fopen(path.c_str(), "r");
    if (file == nullptr) {
        send_error_(make_error_msg(err, ap));
        exit(1);
    }
    char tmp = fgetc(file);
    while (!feof(file)) {
        file_data += tmp;
        tmp = fgetc(file);
    }
    fclose(file);
    va_end(ap);
}
