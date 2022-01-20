#pragma once

#include "base/Error.h"
#include "base/trcdef.h"

namespace trc::utils {
typedef bool (*filefilter)(const std::string&);

/**
 * @brief 遍历目录
 * @param path 遍历的目录
 * @param func
 * 判断文件是否应当存入文件列表的判别函数
 * @param fileList 遍历出文件的存储地
 * @param dirList 遍历出文件夹的存储地
 */
TRC_base_func_api void listfiles(const std::string& path,
    filefilter, vecs& fileList, vecs& dirList);

/**
 * @brief 检查文件是否存在
 */
TRC_base_func_api bool file_exists(const std::string& path);

/**
 * @brief 改变路径后缀名
 * @param path 原路径
 * @param last 需要改变成的后缀名
 */
TRC_base_func_api std::string path_last(
    const std::string& path, const std::string& last);

/**
 * @brief
 * 在虚拟机执行过程中，动态加载字节码时路径转换
 * @code math.lang -> math/lang @endcode
 * @details 不采用正则表达式处理
 */
TRC_base_func_api std::string import_to_path(
    std::string import_name);

/**
 * @brief 将多个路径拼接起来
 * @param n 参数个数
 */
TRC_base_func_api std::string path_join(int n, ...);

/**
 * @brief 获取文件后缀名
 */
TRC_base_func_api std::string file_last_(
    const std::string& path);

/**
 * @brief 读取文件并将字符存入file_data
 * @warning 失败会直接停止程序
 */
TRC_base_func_api void readcode(
    std::string& file_data, const std::string& path);

/**
 * @brief 读取文件并将字符存入file_data
 * @warning 失败后不会直接停止程序
 * @return 成功返回0，失败返回1
 */
TRC_base_func_api int readcode_with_code(
    std::string& file_data, const std::string& path);
}
