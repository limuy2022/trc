/**
 * ctree文件的读取与写入
 * 类似Java的方式，把空间利用最大化
 * 注:大端小端将被统一为大端,因此具备跨平台能力
 */

#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include "TVM/TVM.h"
#include "Error.h"
#include "platform.h"
#include "bytes.h"

// 开头标识文件是否为ctree文件的标识，魔数
static int MAGIC_VALUE = 0xACFD;

// 由于读入和写入是相对的，所以用宏定义来简化这一点
// str:load or write
#define LOAD_WRITE(file, vm, str) \
do{\
    /* 整型常量池 */\
    str##_pool((file), (vm)->static_data.const_i);\
    /* 浮点数常量池 */\
    str##_pool((file), (vm)->static_data.const_f);\
    /* 字符串常量池 */\
    str##_string_pool((file), (vm)->static_data.const_s);\
    /* 长整型常量池 */\
    str##_string_pool((file), (vm)->static_data.const_long);\
    /* 变量名常量池 */\
    str##_string_pool((file), (vm)->static_data.const_name);\
    /* 字节码常量池 */\
    str##_bytecode((file), (vm)->static_data.byte_codes);\
    /* 函数字节码常量池 */\
    str##_functions((file), (vm)->static_data.funcs);\
} while(0)

// 由于大小端统一，要做很多判断，设立函数来简化这一过程，包装fread和fwrite两个函数
void fread_all(void * a, int b, int c, FILE * d) {
    fread(a, b, c, d);
    /*为大端则转成小端*/
    if(!byte_order) {
        bytes_order_change((char*)a, b * c);
    }
}

void fwrite_all(const void * a, int b, int c, FILE * d) {
    fwrite(a, b, c, d);
    /*为小端则转成大端*/
    if(!byte_order) {
        bytes_order_change((char*)a, b * c);
    }
}

#define fread fread_all
#define fwrite fwrite_all

using namespace std;

static string load_string_one(FILE *file) {
    /**
     * 加载单个字符串
     */
    int n;
    fread(&n, sizeof(int), 1, file);
    // 临时申请一段空间用于初始化string
    char *tmp = new char[n];
    fread(tmp, n, 1, file);
    string a(tmp, n);
    delete[]tmp;
    return a;
}

static void write_string_one(FILE *file, const string &data) {
    /**
     * 写入单个字符串
     */
    int n = data.length();
    // 写入数据长度
    fwrite(&n, sizeof(n), 1, file);
    fwrite(data.c_str(), n, 1, file);
}

template<typename T>
static void write_pool(FILE *file, vector<T> &const_pool) {
    /**
     * 写入常量池及常量池大小
     * 注：基础数据类型，其它数据类型可能导致错误
     */
    int size = const_pool.size();
    fwrite(&size, sizeof(size), 1, file);
    for (const auto &i: const_pool)
        fwrite(&i, sizeof(i), 1, file);
}

template<typename T>
static void load_pool(FILE *file, vector<T> &const_pool) {
    /**
     * 读取常量池到TVM中
     * 注：基础数据类型，其它数据类型可能导致错误
     */
    int size;
    fread(&size, sizeof(int), 1, file);
    const_pool.resize(size);
    for (auto &i: const_pool)
        fread(&i, sizeof(i), 1, file);
}

static void write_string_pool(FILE *file, vector<string> &const_pool) {
    /**
     * string是可变长数据类型，需要特殊处理
     */
    int size = const_pool.size();
    // 数据长度
    fwrite(&size, sizeof(size), 1, file);
    for (const auto &i: const_pool)
        write_string_one(file, i);
}

static void write_bytecode(FILE *file, vector<vector<short *> > &const_opcode) {
    /**
     * 写入字节码
     */
    // 字节码条数
    int size = const_opcode.size(), line_size;
    fwrite(&size, sizeof(int), 1, file);
    for (const auto &i: const_opcode) {
        line_size = i.size();
        fwrite(&line_size, sizeof(int), 1, file);
        for (const auto &j: i) {
            fwrite(&j[0], sizeof(j[0]), 1, file);
            fwrite(&j[1], sizeof(j[1]), 1, file);
        }
    }
}

static void load_string_pool(FILE *file, vecs &const_pool) {
    /**
     * string是可变长数据类型，需要特殊处理
     * 注：会清空vector原有的值
     */
    int size;
    // 数据长度
    fread(&size, sizeof(size), 1, file);
    const_pool.resize(size);
    for (auto &i: const_pool)
        i = load_string_one(file);
}

static void load_bytecode(FILE *file, vector<vector<short *> > &const_opcode) {
    /**
     * 读取字节码
     */
    // 字节码条数
    int size, line_size;
    short name, argv;
    fread(&size, sizeof(size), 1, file);
    for (int i = 0; i < size; ++i) {
        fread(&line_size, sizeof(line_size), 1, file);
        vector<short *> line;
        for (int j = 0; j < line_size; ++j) {
            fread(&name, sizeof(name), 1, file);
            fread(&argv, sizeof(argv), 1, file);
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

static void load_functions(FILE *file, map<string, func_ *> &const_funcl) {
    /**
     * 读取函数并创建环境
     */

    int len;
    func_ *tmp;
    fread(&len, sizeof(len), 1, file);
    for (int i = 0; i < len; ++i) {
        const string &t = load_string_one(file);
        tmp = new func_(t);
        const_funcl[t] = tmp;
        load_bytecode(file, tmp->bytecodes);
    }
}

static void write_functions(FILE *file, map<string, func_ *> &const_funcl) {
    /**
     * 写入函数及其环境
     */

    for (const auto &i: const_funcl) {
        write_string_one(file, i.first);
        write_bytecode(file, i.second->bytecodes);
    }
}

bool is_magic(const string &path) {
    /**
     * 判断一个文件魔数是否正确
     * 注：此时文件必须未被打开
     */
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
        send_error(OpenFileError, path.c_str());
    int magic;
    fread(&magic, sizeof(magic), 1, file);
    fclose(file);
    return magic == MAGIC_VALUE;
}

void loader_ctree(TVM *vm, const string &path) {
    /**
     * 读取ctree文件，并对文件进行验证(魔数及版本号)
     */
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
        send_error(OpenFileError, path.c_str());
    // 读取魔数
    // ACFD
    int magic;
    fread(&magic, sizeof(magic), 1, file);
    if (magic != MAGIC_VALUE) {
        cerr << "Trc:\"" << path << "\" is not a ctree file.Because its magic number is error\n";
        exit(1);
    }

    // 读取版本号
    float ver_;
    fread(&ver_, sizeof(ver_), 1, file);
    vm->static_data.ver_ = ver_;
    vm->check_TVM();
    // 开始正式读写
    LOAD_WRITE(file, vm, load);
    fclose(file);
}

void save_ctree(TVM *vm, const string &path) {
    /**
     * 保存虚拟机数据为ctree文件
     * 注意：并不对vm进行任何内存操作
     */
    FILE *file = fopen(path.c_str(), "wb");
    if (file == nullptr) {
        send_error(OpenFileError, path.c_str());
    }
    // 写入魔数
    // ACFD
    fwrite(&MAGIC_VALUE, sizeof(MAGIC_VALUE), 1, file);
    // 写入版本号
    fwrite(&version, sizeof(version), 1, file);
    LOAD_WRITE(file, vm, write);
    fclose(file);
}
