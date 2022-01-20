/**
 * 关于文件系统和文件路径的操作
 */

#include "base/utils/filesys.h"
#include "base/Error.h"
#include "base/trcdef.h"
#include "language/error.h"
#include <cstdarg>
#include <cstdio>
#include <filesystem>
#include <string>
#include <sys/stat.h>

namespace fs = std::filesystem;

namespace trc::utils {
void listfiles(const std::string& path, filefilter func,
    vecs& fileList, vecs& dirList) {
    std::string tmp;
    for (const auto& path_i :
        fs::recursive_directory_iterator(path)) {
        if (is_directory(path_i)) {
            fileList.push_back(path_i.path().string());
            continue;
        }
        tmp = path_i.path().string();
        if (func(tmp)) {
            dirList.push_back(tmp);
        }
    }
}

bool file_exists(const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");
    if (file == nullptr) {
        return false;
    } else {
        fclose(file);
        return true;
    }
}

std::string path_last(
    const std::string& path, const std::string& last) {
    return path.substr(0, path.find_last_of('.')) + last;
}

std::string import_to_path(std::string import_name) {
    for (int i = 0, n = import_name.length(); i < n; ++i) {
        if (import_name[i] == '.') {
            import_name[i] = '/';
        }
    }
    return import_name;
}

std::string path_join(int n, ...) {
    va_list ap;
    va_start(ap, n);
    std::string root(va_arg(ap, const char*));
    for (int i = 1; i < n; ++i) {
        root += "/";
        root += va_arg(ap, const char*);
    }
    va_end(ap);
    return root;
}

std::string file_last_(const std::string& path) {
    return path.substr(path.find_last_of('.'));
}

/**
 * @brief 具体的读取文件的细节
 * @param path 文件路径
 * @param file_data 读取出的内容的存放地
 * @param file 文件
 */
inline void read_file_detail(const std::string& path,
    std::string& file_data, FILE* file) {
    struct stat buffer;
    stat(path.c_str(), &buffer);
    size_t size = buffer.st_size;
    file_data.resize(size);
    fread(
        (char*)file_data.c_str(), sizeof(char), size, file);
}

void readcode(
    std::string& file_data, const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");
    if (!file)
        error::send_error(error::OpenFileError,
            language::error::openfileerror, path.c_str());
    read_file_detail(path, file_data, file);
    fclose(file);
}

int readcode_with_code(
    std::string& file_data, const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");
    if (!file) {
        return 1;
    }
    read_file_detail(path, file_data, file);
    fclose(file);
    return 0;
}
}
