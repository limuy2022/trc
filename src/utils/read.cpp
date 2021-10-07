#include <string>
#include <fstream>
#include <iostream>
#include <cstdarg>
#include "../include/Error.h"

using namespace std;

void readcode(string &file_data, const string &path) {
    /*
    * 读取文件并返回字符串
    */

    file_data.clear();
    ifstream file(path);
    string temp;
    if(!file.is_open())
        send_error(OpenFileError, path.c_str());
    
    while (getline(file, temp)) {
        file_data += temp;
        file_data += "\n";
    }
    file.close();
}


void readfile(string &file_data, const string &path, const int err, ...) {
    /**
     * 与上面那种方法不同在于可以自由选择报错类型，但失败同样会结束程序
     * err：错误名
     */

    file_data.clear();
    va_list ap;
    va_start(ap, &err);
    ifstream file(path);
    string temp;
    if(!file.is_open()) {
        send_error_(make_error_msg(err, ap));
        exit(1);
    }
    while (getline(file, temp)) {
        file_data += temp;
        file_data += "\n";
    }
    file.close();
    va_end(ap);
}
