#pragma once

#include <base/Error.h>
#include <base/trcdef.h>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace trc::utils {
/**
 * @brief 遍历目录
 */
class TRC_base_api listfiles{
public:
    /**
     * @param path 遍历的目录
     */
    listfiles(const std::string& path);

    /**
     * @return 获取下一项，为空字符串说明到头了
     */
    fs::path nextitem();
private:
    fs::recursive_directory_iterator iter, now;
};

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
