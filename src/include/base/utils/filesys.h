#pragma once

#include <base/Error.h>
#include <base/trcdef.h>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace trc::utils {
// 用来过滤文件的函数类型
typedef bool (*filefilter)(const std::filesystem::path&);

/**
 * @brief 遍历目录
 * @param path 遍历的目录
 * @param func
 * 判断文件是否应当存入文件列表的判别函数
 * 默认全部加入
 * @param fileList 遍历出文件的存储地
 * @param dirList 遍历出文件夹的存储地
 */
TRC_base_c_api void listfiles(
    const std::string& path, std::vector<fs::path>& fileList,
    std::vector<fs::path>& dirList,
    filefilter = [](const fs::path&) -> bool { return true; });

/**
 * @brief 检查文件是否存在
 */
TRC_base_c_api bool file_exists(const std::string& path);

/**
 * @brief
 * 在虚拟机执行过程中，动态加载字节码时路径转换
 * @code math.lang -> math/lang @endcode
 * @details 不采用正则表达式处理
 */
TRC_base_c_api void import_to_path(std::string& import_name);

/**
 * @brief 读取文件并将字符存入file_data
 * @warning 失败会直接停止程序
 */
TRC_base_c_api void readcode(std::string& file_data, const std::string& path);

/**
 * @brief 读取文件并将字符存入file_data
 * @warning 失败后不会直接停止程序
 * @return 成功返回0，失败返回1
 */
TRC_base_c_api int readcode_with_code(
    std::string& file_data, const std::string& path);

/**
 * @brief 将指定长度的字符串写入文件
 */
TRC_base_c_api void write_to_file(FILE* file, const char* str, size_t len);
}
