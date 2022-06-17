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
#include <cstdarg>
#include <cstdio>
#include <filesystem>
#include <language/error.h>
#include <string>
#include <sys/stat.h>

namespace fs = std::filesystem;

namespace trc::utils {
void listfiles(const std::string& path, std::vector<fs::path>& fileList,
    std::vector<fs::path>& dirList, filefilter func) {
    fs::path tmp;
    for (const auto& path_i : fs::recursive_directory_iterator(path)) {
        tmp = path_i.path();
        if (is_directory(path_i)) {
            dirList.push_back(tmp);
        } else if (func(tmp.filename())) {
            fileList.push_back(tmp);
        }
    }
}

bool file_exists(const std::string& path) {
    if (fs::exists(path)) {
        if (fs::is_directory(path)) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

std::string import_to_path(std::string import_name) {
    for (int i = 0, n = import_name.length(); i < n; ++i) {
        if (import_name[i] == '.') {
            import_name[i] = '/';
        }
    }
    return import_name;
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
}
