/**
 * @file filesys.cpp
 * @brief 关于文件系统和文件路径的操作
 * @date 2022-06-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <base/Error.h>
#include <base/trcdef.h>
#include <base/utils/filesys.h>
#include <cstdio>
#include <filesystem>
#include <language/error.h>
#include <string>
#include <sys/stat.h>

namespace fs = std::filesystem;

namespace trc::utils {
listfiles::listfiles(const std::string& path):iter(path), now(begin(iter)) {}

fs::path listfiles::nextitem() {
    now++;
    if(now == end(iter)) {
        return "";
    }
    return now->path();
}

bool file_exists(const std::string& path) {
    // 如果存在且不为目录，返回true
    return fs::exists(path) ? !fs::is_directory(path) : false;
}

void import_to_path(std::string& import_name) {
    for (char& i : import_name) {
        if (i == '.') {
            i = '/';
        }
    }
}

/**
 * @brief 具体的读取文件的细节
 * @param path 文件路径
 * @param file_data 读取出的内容的存放地
 * @param file 文件
 */
inline void read_file_detail(
    const std::string& path, std::string& file_data, FILE* file) {
    struct stat buffer;
    stat(path.c_str(), &buffer);
    size_t size = buffer.st_size;
    file_data.resize(size + 1);
    file_data[size] = '\0';
    fread((char*)file_data.c_str(), sizeof(char), size, file);
}

void readcode(std::string& file_data, const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");
    if (file == nullptr) {
        error::send_error(
            error::OpenFileError, language::error::openfileerror, path.c_str());
    }
    read_file_detail(path, file_data, file);
    fclose(file);
}

int readcode_with_code(std::string& file_data, const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");
    if (file == nullptr) {
        return 1;
    }
    read_file_detail(path, file_data, file);
    fclose(file);
    return 0;
}

void write_to_file(FILE* file, const char* str, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        fputc(str[i], file);
    }
}
}
