#include <string>
#include <cstdio>

/**
 * @brief 将文件修正到正确的目录
 * @param p 以testdata为当前目录编写的相对路径
 * @return 正确的以bin为当前目录的相对路径
 */
std::string redefine_path(const std::string& p);

/**
 * @brief 打开文件，错误就报出测试资源缺失错误,使用按fopen使用
 */
FILE* fopen_with_check(const char* path, const char* mode);
