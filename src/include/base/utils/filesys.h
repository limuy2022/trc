#pragma once

#include <base/Error.h>
#include <base/trcdef.h>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace trc::utils {
/**
 * @brief
 * 在虚拟机执行过程中，动态加载字节码时路径转换
 * @code math.lang -> math/lang @endcode
 * @details 不采用正则表达式处理
 */
void import_to_path(std::string& import_name);

/**
 * @brief 读取文件并将字符存入file_data
 * @warning 失败会直接停止程序
 */
void readcode(std::string& file_data, const std::string& path);

/**
 * @brief 读取文件并将字符存入file_data
 * @warning 失败后不会直接停止程序
 * @return 成功返回0，失败返回1
 */
int readcode_with_code(std::string& file_data, const std::string& path);
}
