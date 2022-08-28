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
#include <cstring>
#include <language/error.h>
#include <platform.h>
#include <string>
#include <vector>

// 开头标识文件是否为ctree文件的标识，魔数
static uint16_t MAGIC_VALUE = 0xACFD;

/**
 * 由于读入和写入是相对的，所以用宏定义来简化这一点
 * @param str load or write
 */
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
        /* 符号表 */                                                        \
        str##_var_form((file), (vm)->static_data.global_symbol_table_size);    \
        /* 字节码常量池 */                                               \
        str##_bytecode((file), (vm)->static_data);                             \
        /* 函数字节码常量池 */                                         \
        str##_functions((file), (vm)->static_data.funcs);                      \
    } while (0)

namespace trc::loader {
/**
 * @brief
 * 包装fread函数，使它能够自动统一为大端字节序，用法与fread相同
 * @warning 字符串等数据无需通过此种方式写入
 */
static void fread_cross_plat(void* a, size_t b, size_t c, FILE* d) {
    fread(a, b, c, d);
    /*为大端则转成小端*/
    if (!trc::def::byte_order) {
        trc::utils::bytes_order_change((trc::def::byte_t*)a, b * c);
    }
}

/**
 * @brief
 * 包装fwrite函数，使它能够自动统一为大端字节序，用法与fwrite相同
 * @warning 字符串等数据无需通过此种方式读取
 */
static void fwrite_cross_plat(const void* a, size_t b, size_t c, FILE* d) {
    if (!def::byte_order) {
        /*为小端则转成大端*/
        utils::bytes_order_change((def::byte_t*)a, b * c);
        fwrite(a, b, c, d);
        utils::bytes_order_change((def::byte_t*)a, b * c);
    } else {
        fwrite(a, b, c, d);
    }
}

/**
 * @brief 加载单个字符串
 */
static char* load_string_one(FILE* file) {
    uint32_t n;
    fread_cross_plat(&n, sizeof(n), 1, file);
    char* res = new char[n + 1];
    fread(res, n, sizeof(char), file);
    res[n] = '\0';
    return res;
}

/**
 * @brief 写入单个字符串
 */
static void write_string_one(FILE* file, const char* data) {
    auto n = static_cast<uint32_t>(strlen(data));
    // 写入数据长度
    fwrite_cross_plat(&n, sizeof(n), 1, file);
    fwrite(data, n, sizeof(char), file);
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
    uint32_t size = const_pool.size();
    fwrite_cross_plat(&size, sizeof(size), 1, file);
    for (const auto& i : const_pool) {
        fwrite_cross_plat(&i, sizeof(T), 1, file);
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
    uint32_t size;
    fread_cross_plat(&size, sizeof(size), 1, file);
    const_pool.resize(size);
    for (uint32_t i = 0; i < size; --i) {
        fread_cross_plat(&const_pool[i], sizeof(T), 1, file);
    }
}

/**
 * @brief 向文件写入字符串型常量池
 * @warning
 * string是可变长数据类型，需要特殊处理，仅可使用本函数写入
 * @param file 文件
 * @param const_pool 字符串型常量池
 */
static void write_string_pool(
    FILE* file, std::vector<const char*>& const_pool) {
    // 数据长度
    auto size = static_cast<uint32_t>(const_pool.size());
    fwrite_cross_plat(&size, sizeof(size), 1, file);
    for (auto i : const_pool)
        write_string_one(file, i);
}

/**
 * @brief 从文件读入字符串型常量池
 * @warning 会清空vector原有的值
 * @warning
 * string是可变长数据类型，需要特殊处理，仅可使用本函数读入
 * @param file 文件
 * @param const_pool 字符串型常量池
 */
static void load_string_pool(FILE* file, std::vector<const char*>& const_pool) {
    const_pool.clear();
    uint32_t size;
    // 数据长度
    fread_cross_plat(&size, sizeof(size), 1, file);
    const_pool.reserve(size);
    while (size--) {
        const_pool.push_back(load_string_one(file));
    }
}

/**
 * @brief 读入符号表大小
 * @param file 文件
 * @param table_size 符号表大小
 */
static void load_var_form(FILE* file, size_t& table_size) {
    fread_cross_plat(&table_size, sizeof(table_size), 1, file);
}

/**
 * @brief 写入符号表大小
 */
static void write_var_form(FILE* file, size_t table_size) {
    fwrite_cross_plat(&table_size, sizeof(table_size), 1, file);
}

/**
 * @brief 向文件写入字节码和行号表
 * @param file 文件
 * @param static_data 静态数据
 */
static void write_bytecode(
    FILE* file, trc::TVM_space::TVM_static_data& static_data) {
    // 字节码条数
    auto size = (uint32_t)static_data.byte_codes.size();
    fwrite_cross_plat(&size, sizeof(size), 1, file);
    // 写入行号表
    size_t line_numeber_size = static_data.line_number_table.size();
    fwrite_cross_plat(&line_numeber_size, sizeof(line_numeber_size), 1, file);
    for (size_t i = 0; i < line_numeber_size; ++i) {
        fwrite_cross_plat(&static_data.line_number_table[i],
            sizeof(decltype(static_data.line_number_table)::value_type), 1,
            file);
    }
    // 具体字节码
    for (const auto& i : static_data.byte_codes) {
        fwrite_cross_plat(&i.bycode, sizeof(i.bycode), 1, file);
        fwrite_cross_plat(&i.index, sizeof(i.index), 1, file);
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
    uint32_t size;
    fread_cross_plat(&size, sizeof(size), 1, file);
    // 读取长度判断是否有行号表
    size_t line_number_size;
    fread_cross_plat(&line_number_size, sizeof(line_number_size), 1, file);
    if (size) {
        static_data.line_number_table.reserve(line_number_size);
        decltype(static_data.line_number_table)::value_type tmp;
        for (size_t i = 0; i < line_number_size; ++i) {
            fread_cross_plat(&tmp,
                sizeof(decltype(static_data.line_number_table)::value_type), 1,
                file);
            static_data.line_number_table.push_back(tmp);
        }
    }
    // 读取具体字节码
    trc::TVM_space::bytecode_t name;
    trc::TVM_space::bytecode_index_t argv;
    static_data.byte_codes.reserve(size);
    for (uint32_t i = 0; i < size; ++i) {
        fread_cross_plat(&name, sizeof(name), 1, file);
        fread_cross_plat(&argv, sizeof(argv), 1, file);
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
static void load_functions(FILE* file, trc::TVM_space::func_*& const_funcl) {
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
static void write_functions(FILE* file, trc::TVM_space::func_* const_funcl) {
    // for (const auto &i: const_funcl) {
    //     write_string_one(file, i.first);
    //     write_bytecode(file,
    //     i.second->bytecodes);
    // }
}

static int getbigversion(const char* version) {
    int res = 0;
    for (int i = 0; version[i] != '.'; ++i) {
        res *= 10;
        res += version[i] - '0';
    }
    return res;
}

/**
 * @brief 比较传入版本号是否合法
 * @details 合法的标准是程序大版本号大于传入的版本号，小版本号可以不相同
 * @return true 合法的版本号
 * @return false 不合法的版本号
 */
static bool compare_version(const char* version) {
    return getbigversion(trc::def::version) >= getbigversion(version);
}

/**
 * @brief 判断是不是ctree文件的内部实现
 * @param file
 * @return true 代表是真实的ctree文件
 */
static bool is_magic_interal(FILE* file) {
    decltype(MAGIC_VALUE) magic;
    fread_cross_plat(&magic, sizeof(magic), 1, file);
    return magic == MAGIC_VALUE;
}

bool is_magic(const std::string& path) {
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
        error::send_error(
            error::OpenFileError, language::error::openfileerror, path.c_str());
    bool res = is_magic_interal(file);
    fclose(file);
    return res;
}

void loader_ctree(TVM_space::TVM* vm, const std::string& path) {
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
        error::send_error(
            error::OpenFileError, language::error::openfileerror, path.c_str());
    // 读取魔数
    if (!is_magic_interal(file)) {
        fprintf(stderr, language::error::magic_value_error, path.c_str());
        exit(EXIT_FAILURE);
    }
    // 读取版本号长度
    uint32_t version_size;
    fread_cross_plat(&version_size, sizeof(version_size), 1, file);
    // 读取版本号
    char* ver_ = new char[version_size + 1];
    fread(ver_, sizeof(version_size), sizeof(char), file);
    ver_[version_size] = '\0';
    if (!compare_version(def::version)) {
        error::send_error(error::VersionError, language::error::versionerror,
            ver_, def::version);
    }
    delete[] ver_;
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
    fwrite_cross_plat(&MAGIC_VALUE, sizeof(MAGIC_VALUE), 1, file);
    // 写入版本号的长度
    auto size_tmp = uint32_t(strlen(def::version));
    fwrite_cross_plat(&size_tmp, sizeof(size_tmp), 1, file);
    // 写入版本号
    fwrite(def::version, strlen(def::version), sizeof(char), file);
    LOAD_WRITE(file, vm, write);
    fclose(file);
}
}
