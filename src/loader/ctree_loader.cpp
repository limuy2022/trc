/**
 * ctree文件的读取与写入
 * 类似Java的方式，把空间利用最大化
 * 注:写入时随着系统和编译器而改变字节序,大端小端难以确定,因此不具备跨平台能力(包括编译器)
 */

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "../include/TVM/TVM.h"
#include "../include/Error.h"

// 开头标识文件是否为ctree文件的标识，魔数
#define MAGIC_VALUE 0xACFD
// 读写时需要转化的类型
#define RWTYPE (char*)

// 由于读入和写入是相对的，所以用宏定义来简化这一点
// str:load or write
#define LOAD_WRITE(file, vm, str) \
do{\
    str##_pool((file), (vm)->static_data.const_i);\
    str##_pool((file), (vm)->static_data.const_f);\
    str##_string_pool((file), (vm)->static_data.const_s);\
    str##_string_pool((file), (vm)->static_data.const_long);\
    str##_string_pool((file), (vm)->static_data.const_name);\
    str##_bytecode((file), (vm)->static_data.byte_codes);\
    str##_functions((file), (vm)->static_data.funcs);\
} while(0)

using namespace std;

static string load_string_one(ifstream &file) {
    /**
     * 加载单个字符串
     */ 
    int n;
    file.read(RWTYPE &n, sizeof(int));
    // 临时申请一段空间用于初始化string
    char *tmp = new char[n];
    file.read(tmp, n);
    string a(tmp, n);
    delete []tmp;
    return a;
}

static void write_string_one(ofstream &file, const string &data) {
    /**
     * 写入单个字符串
     */ 
    int n = data.length();
    // 写入数据长度
    file.write(RWTYPE &n, sizeof(int));
    file.write(data.c_str(), n);
}

namespace {
    template<typename T>
    void write_pool(ofstream &file, vector<T> &const_pool) {
        /**
         * 写入常量池及常量池大小
         * 注：基础数据类型，其它数据类型可能导致错误
         */
        int size = const_pool.size();
        file.write(RWTYPE &size, sizeof(int));
        for (const auto &i:const_pool)
            file.write(RWTYPE &i, sizeof(i));
    }

    template<typename T>
    void load_pool(ifstream &file, vector<T> &const_pool) {
        /**
         * 读取常量池到TVM中
         * 注：基础数据类型，其它数据类型可能导致错误
         */
        int size;
        file.read(RWTYPE &size, sizeof(int));
        const_pool.resize(size);
        for (auto &i:const_pool)
            file.read(RWTYPE &i, sizeof(i));
    }
}

static void write_string_pool(ofstream &file, vector<string> &const_pool) {
    /**
     * string是可变长数据类型，需要特殊处理
     */
    int size = const_pool.size();
    // 数据长度
    file.write(RWTYPE &size, sizeof(int));
    for (const auto& i : const_pool)
        write_string_one(file, i);
}

static void write_bytecode(ofstream &file, vector<vector<short *> > &const_opcode) {
    /**
     * 写入字节码
     */
    // 字节码条数
    int size = const_opcode.size(), line_size;
    file.write(RWTYPE &size, sizeof(int));
    for (const auto& i : const_opcode) {
        line_size = i.size();
        file.write(RWTYPE &line_size, sizeof(int));
        for (const auto& j : i) {
            file.write(RWTYPE &j[0], sizeof(j[0]));
            file.write(RWTYPE &j[1], sizeof(j[1]));
        }
    }
}

static void load_string_pool(ifstream &file, vecs &const_pool) {
    /**
     * string是可变长数据类型，需要特殊处理
     * 注：会清空vector原有的值
     */
    int size;
    // 数据长度
    file.read(RWTYPE &size, sizeof(size));
    const_pool.resize(size);
    for (auto &i : const_pool)
        i = load_string_one(file);
}

static void load_bytecode(ifstream &file, vector<vector<short *> > &const_opcode) {
    /**
     * 读取字节码
     */
    // 字节码条数
    int size, line_size;
    short name, argv;
    file.read(RWTYPE &size, sizeof(size));
    for (int i = 0; i < size; ++i) {
        file.read(RWTYPE &line_size, sizeof(line_size));
        vector<short *> line;
        for (int j = 0; j < line_size; ++j) {
            file.read(RWTYPE &name, sizeof(name));
            file.read(RWTYPE &argv, sizeof(argv));
            line.push_back(new short[2]{name, argv});
        }
        const_opcode.push_back(line);
    }
}

/**
 * 函数写入字节码文件规定
 * 首先写入函数名长度，再写入函数名
 * 接着写入起始地址和终止地址
 */

static void load_functions(ifstream &file, map<string, func_*> &const_funcl) {
    /**
     * 读取函数并创建环境
     */

    int len;
    string t;
    func_* tmp;
    file.read(RWTYPE &len, sizeof(len));
    for(int i = 0; i < len; ++i) {
        t = load_string_one(file);
        tmp = new func_(t);
        const_funcl[t] = tmp;
        load_bytecode(file, tmp -> bytecodes);
    }
}

static void write_functions(ofstream &file, map<string, func_*> &const_funcl) {
    /**
     * 写入函数及其环境
     */

    for(const auto& i : const_funcl) {
        write_string_one(file, i.first);
        write_bytecode(file, i.second -> bytecodes);
    }
}

bool is_magic(const string &path) {
    /**
     * 判断一个文件魔数是否正确
     * 注：此时文件必须未被打开
     */
    ifstream file(path, ios::binary);
    if (!file.is_open())
        send_error(OpenFileError, path.c_str());
    int magic;
    file.read(RWTYPE &magic, sizeof(magic));
    file.close();
    return magic == MAGIC_VALUE;
}

void loader_ctree(TVM *vm, const string &path) {
    /**
     * 读取ctree文件，并对文件进行验证(魔数及版本号)
     */
    ifstream file(path, ios::binary);
    if (!file.is_open())
        send_error(OpenFileError, path.c_str());
    // 读取魔数
    // ACFD
    int magic;
    file.read(RWTYPE &magic, sizeof(magic));
    if (magic != MAGIC_VALUE) {
        cerr << "Trc:\"" << path << "\" is not a ctree file.Because its magic number is error\n";
        exit(1);
    }

    // 读取版本号
    float ver_;
    file.read(RWTYPE &ver_, sizeof(ver_));
    vm->static_data.ver_ = ver_;
    vm->check_TVM();
    LOAD_WRITE(file, vm, load);
    file.close();
}

void save_ctree(TVM *vm, const string &path) {
    /**
     * 保存虚拟机数据为ctree文件
     * 注意：并不对vm进行任何内存操作
     */
    ofstream file(path, ios::binary);
    if (!file.is_open()) {
        send_error(OpenFileError, path.c_str());
    }
    // 写入魔数
    // ACFD
    int magic = MAGIC_VALUE;
    file.write(RWTYPE &magic, sizeof(magic));
    // 写入版本号
    file.write(RWTYPE &version, sizeof(version));
    LOAD_WRITE(file, vm, write);
    file.close();
}

#undef MAGIC_VALUE
#undef RWTYPE
#undef LOAD_WRITE
