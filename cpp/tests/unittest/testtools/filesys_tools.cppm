module;
#include <cstdio>
#include <cstdlib>
#include <string>
export module filesys_tools;

namespace tools {
/**
 * @brief 将文件修正到正确的目录
 * @param p 以testdata为当前目录编写的相对路径
 * @return 正确的以bin为当前目录的相对路径
 */
export std::string redefine_path(const std::string& p) {
    return "../tests/unittest/testdata/" + p;
}

/**
 * @brief
 * 打开文件，错误就报出测试资源缺失错误,使用按fopen使用
 */
export FILE* fopen_with_check(const char* path, const char* mode) {
    FILE* file = fopen(path, mode);
    if (file == nullptr) {
        fprintf(stderr,
            "The test data has been lost.Unable to "
            "continue testing.\n");
        exit(EXIT_FAILURE);
    }
    return file;
}
}
