#pragma once
#include <TVM/TVM.h>
#include <base/library.h>
#include <string>

namespace trc::loader {
/**
 * @brief
 * 读取ctree文件到虚拟机中，并对文件进行验证(魔数及版本号)
 * @param vm 虚拟机
 * @param path 路径
 */
TRC_base_c_api void loader_ctree(
    TVM_space::TVM* vm, const std::string& path);

/**
 * @brief 保存虚拟机数据为ctree文件
 * @param vm 虚拟机
 * @param path 路径
 */
TRC_base_c_api void save_ctree(TVM_space::TVM* vm, const std::string& path);

/**
 * @brief 判断一个文件是不是合法的ctree文件
 * @param path 路径
 * @warning 此时文件必须未被打开
 */
TRC_base_c_api bool is_magic(const std::string& path);
}
