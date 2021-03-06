/**
 * ctree文件的读取与写入
 * 类似Java的方式，把空间利用最大化
 * 注:大端小端将被统一为大端,因此具备跨平台能力
 */

#include <TVM/TVM.h>
#include <TVM/func.h>
#include <base/Error.h>
#include <base/ctree_loader.h>
#include <base/utils/bytes.h>
#include <cstdio>
#include <language/error.h>
#include <platform.h>
#include <string>
#include <vector>

// 开头标识文件是否为ctree文件的标识，魔数
static const int MAGIC_VALUE = 0xACFD;

// 由于读入和写入是相对的，所以用宏定义来简化这一点
// str:load or write
#define LOAD_WRITE(file, vm, str)                                              \
    do {                                                                       \
        /* 整型常量池 */                                                  \
        str##_pool((file), (vm)->static_data.const_i);                         \
        /* 浮点数常量池 */                                               \
        str##_pool((file), (vm)->static_data.const_f);                         \
        /* 字符串常量池 */                                               \
        str##_string_pool((file), (vm)->static_data.const_s);                  \
        /* 长整型常量池 */                                               \
        str##_string_pool((file), (vm)->static_data.const_long);               \
        /* 变量名常量池 */                                               \
        str##_var_form((file), (vm)->static_data.global_symbol_table_size);    \
        /* 字节码常量池 */                                               \
        str##_bytecode((file), (vm)->static_data);                             \
        /* 函数字节码常量池 */                                         \
        str##_functions((file), (vm)->static_data.funcs);                      \
    } while (0)

/**
 * @brief
 * 包装fread函数，使它能够自动统一为大端字节序
 * @warning 用法与fread相同
 */
static void fread_all(void* a, size_t b, size_t c, FILE* d) {
    fread(a, b, c, d);
    /*为大端则转成小端*/
    if (!trc::def::byte_order) {
        trc::utils::bytes_order_change((trc::def::byte_t*)a, b * c);
    }
}

/**
 * @brief
 * 包装fwrite函数，使它能够自动统一为大端字节序
 * @warning 用法与fwrite相同
 */
static void fwrite_all(const void* a, size_t b, size_t c, FILE* d) {
    fwrite(a, b, c, d);
    /*为小端则转成大端*/
    if (!trc::def::byte_order) {
        trc::utils::bytes_order_change((trc::def::byte_t*)a, b * c);
    }
}

#define fread fread_all
#define fwrite fwrite_all

/**
 * @brief 加载单个字符串
 */
static char* load_string_one(FILE* file) {
    int n;
    fread(&n, sizeof(n), 1, file);
    char* res = new char[n + 1];
    fread(res, n, 1, file);
    res[n] = '\0';
    return res;
}

/**
 * @brief 写入单个字符串
 */
static void write_string_one(FILE* file, const std::string& data) {
    int n = (int)data.length();
    // 写入数据长度
    fwrite(&n, sizeof(n), 1, file);
    fwrite(data.c_str(), n, sizeof(char), file);
}

/**
 * @brief 向文件写入常量池数据及常量池大小
 * @warning
 * 仅限基础数据类型，其它数据类型可能导致错误
 * @param file 文件
 * @param const_pool 常量池
 */
template <typename T>
static void write_pool(FILE* file, std::vector<T>& const_pool) {
    int size = const_pool.size() - 1;
    fwrite(&size, sizeof(size), 1, file);
    if (size > 0) {
        fwrite(const_pool.data() + 1, sizeof(T), size, file);
    }
}

/**
 * @brief 从文件中读取常量池
 * @warning 基础数据类型，其它数据类型可能导致错误
 * @param file 文件
 * @param const_pool 常量池
 */
template <typename T>
static void load_pool(FILE* file, std::vector<T>& const_pool) {
    int size;
    fread(&size, sizeof(size), 1, file);
    // 留出占位的位置
    const_pool.resize(size + 1);
    fread(const_pool.data() + 1, sizeof(T), size, file);
}

/**
 * @brief 向文件写入字符串型常量池
 * @warning
 * string是可变长数据类型，需要特殊处理，仅可使用本函数写入
 * @param file 文件
 * @param const_pool 字符串型常量池
 */
static void write_string_pool(FILE* file, std::vector<char*>& const_pool) {
    // 减去开头占位的数据
    int size = (int)const_pool.size() - 1;
    // 数据长度
    fwrite(&size, sizeof(size), 1, file);
    for (size_t i = 1, n = const_pool.size(); i < n; ++i)
        write_string_one(file, const_pool[i]);
}

/**
 * @brief 从文件读入字符串型常量池
 * @warning 会清空vector原有的值
 * @warning
 * string是可变长数据类型，需要特殊处理，仅可使用本函数读入
 * @param file 文件
 * @param const_pool 字符串型常量池
 */
static void load_string_pool(FILE* file, std::vector<char*>& const_pool) {
    const_pool.clear();
    int size;
    // 数据长度
    fread(&size, sizeof(size), 1, file);
    const_pool.reserve(size);
    const_pool.push_back(nullptr);
    for (; size != 0; --size) {
        const_pool.push_back(load_string_one(file));
    }
}

/**
 * @brief 读入符号表大小
 * @param file 文件
 * @param table_size 符号表大小
 */
static void load_var_form(FILE* file, size_t& table_size) {
    fread(&table_size, sizeof(table_size), 1, file);
}

/**
 * @brief 写入符号表大小
 */
static void write_var_form(FILE* file, size_t table_size) {
    fwrite(&table_size, sizeof(table_size), 1, file);
}

/**
 * @brief 向文件写入字节码和行号表
 * @param file 文件
 * @param static_data 静态数据
 */
static void write_bytecode(
    FILE* file, trc::TVM_space::TVM_static_data& static_data) {
    // 字节码条数
    int size = (int)static_data.byte_codes.size();
    fwrite(&size, sizeof(size), 1, file);
    // 是否带有行号表
    size_t line_numeber_size = static_data.line_number_table.size();
    fwrite(&line_numeber_size, sizeof(line_numeber_size), 1, file);
    if (size != 0) {
        // 为是，写入行号表
        fwrite(static_data.line_number_table.data(),
            sizeof(decltype(static_data.line_number_table)::value_type),
            line_numeber_size, file);
    }
    // 具体字节码
    for (const auto& i : static_data.byte_codes) {
        fwrite(&i.bycode, sizeof(i.bycode), 1, file);
        fwrite(&i.index, sizeof(i.index), 1, file);
    }
}

/**
 * @brief 从文件读取字节码和行号表
 * @param file 文件
 * @param static_data 静态数据存放变量
 */
static void load_bytecode(
    FILE* file, trc::TVM_space::TVM_static_data& static_data) {
    // 字节码条数
    int size;
    trc::TVM_space::bytecode_t name;
    trc::TVM_space::bytecode_index_t argv;
    fread(&size, sizeof(size), 1, file);
    // 读取长度判断是否有行号表
    size_t line_number_size;
    fread(&line_number_size, sizeof(line_number_size), 1, file);
    if (size) {
        static_data.line_number_table.resize(line_number_size);
        fread(static_data.line_number_table.data(),
            sizeof(decltype(static_data.line_number_table)::value_type),
            line_number_size, file);
    }
    // 读取具体字节码
    static_data.byte_codes.reserve(size);
    for (int i = 0; i < size; ++i) {
        fread(&name, sizeof(name), 1, file);
        fread(&argv, sizeof(argv), 1, file);
        static_data.byte_codes.emplace_back(name, argv);
    }
}

/**
 * 函数写入字节码文件规定
 * 首先写入函数名长度，再写入函数名
 * 接着写入起始地址和终止地址
 */

/**
 * @brief 从文件中读取函数
 * @param file 文件
 */
static void load_functions(
    FILE* file, std::map<std::string, trc::TVM_space::func_*>& const_funcl) {
    // int len;
    // trc::TVM_space::func_ *tmp;
    // fread(&len, sizeof(len), 1, file);
    // for (int i = 0; i < len; ++i) {
    //     char*t = load_string_one(file);
    //     tmp = new trc::TVM_space::func_{t};
    //     const_funcl[t] = tmp;
    //     load_bytecode(file, tmp->bytecode);
    // }
}

/**
 * @brief 写入函数
 * @param file 文件
 */
static void write_functions(
    FILE* file, std::map<std::string, trc::TVM_space::func_*>& const_funcl) {
    // for (const auto &i: const_funcl) {
    //     write_string_one(file, i.first);
    //     write_bytecode(file,
    //     i.second->bytecodes);
    // }
}

namespace trc::loader {
bool is_magic(const std::string& path) {
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
        error::send_error(
            error::OpenFileError, language::error::openfileerror, path.c_str());
    int magic;
    fread(&magic, sizeof(magic), 1, file);
    fclose(file);
    return magic == MAGIC_VALUE;
}

void loader_ctree(TVM_space::TVM* vm, const std::string& path) {
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
        error::send_error(
            error::OpenFileError, language::error::openfileerror, path.c_str());
    // 读取魔数
    // ACFD
    int magic;
    fread(&magic, sizeof(magic), 1, file);
    if (magic != MAGIC_VALUE) {
        fprintf(stderr, language::error::magic_value_error, path.c_str());
        exit(1);
    }

    // 读取版本号
    float ver_;
    fread(&ver_, sizeof(ver_), 1, file);
    if (ver_ > def::version) {
        error::send_error(error::VersionError, language::error::versionerror,
            std::to_string(ver_).c_str(), std::to_string(def::version).c_str());
    }
    vm->static_data.ver_ = ver_;
    // 开始正式读写
    LOAD_WRITE(file, vm, load);
    fclose(file);
}

void save_ctree(TVM_space::TVM* vm, const std::string& path) {
    FILE* file = fopen(path.c_str(), "wb");
    if (file == nullptr) {
        error::send_error(
            error::OpenFileError, language::error::openfileerror, path.c_str());
    }
    // 写入魔数
    // ACFD
    fwrite(&MAGIC_VALUE, sizeof(MAGIC_VALUE), 1, file);
    // 写入版本号
    fwrite(&def::version, sizeof(def::version), 1, file);
    LOAD_WRITE(file, vm, write);
    fclose(file);
}
}
