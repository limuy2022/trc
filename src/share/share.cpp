/**
 * 共享数据
 */ 
#include <string>
#include <map>
#include "../include/share.h"
#include "../include/cfg.h"

using namespace std;

const float version = 0.2;

const vecc num = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', '-'};

// 类型以及它们在虚拟机中的数字代号
namespace type_int {
    map<string, int> name_int_s = {
            {"int",    INT_T},
            {"bool",   BOOL_T},
            {"string", STRING_T},
            {"float",  FLOAT_T},
            {"long int", BIGNUM_T},
            {"long float", BIGFLOAT_T},
    };

    string int_name_s[] = {
            "int",
            "bool",
            "string",
            "float",
            "long int",
            "long float",
    };
}

namespace run_env {
    // 执行环境

    // 当前执行模块
    string run_module("__main__");
    // 当前执行到的模块函数
    map<string, int> lines;

    void set_module(const string &name) {
        /**
         * 设置当前编译模块(新建)
         * name:模块名
         */ 
    
        run_env::run_module = name;
        run_env::lines[name] = 0;
    }
};
