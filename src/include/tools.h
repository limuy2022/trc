/**
 * 声明trc定义的所有外调tools函数
 * tools格式：由一个供调用的函数以及一个函数依赖的基本实现
 * 例如：clean,__clean
 */

#pragma once

#include <string>

namespace trc {
namespace TVM_space {
    class TVM;
}

namespace tools {
    // 命令行参数
    extern char** argv;
    // 命令行参数个数
    extern int argc;
    /**
     * @brief 工具内部实现
     */
    namespace tools_in {
        void __brun(
            TVM_space::TVM* vm, const std::string& path);

        /**
         * @brief 负责编译工作的底层函数
         * @param vm 填充的虚拟机
         * @param path 文件的路径
         */
        void __build(
            TVM_space::TVM* vm, const std::string& path);

        void __dis(
            TVM_space::TVM* vm, const char* file_path);

        void __out_grammar(const std::string& path);

        void __run(
            TVM_space::TVM* vm, const std::string& path);

        void __out_token(const std::string& path);
    }

    /**
     * @brief 工具外部接口
     */
    namespace tools_out {
        /**
         * @brief 编译运行，并将编译结果写入文件
         */
        void brun();

        void build();

        void dis();

        /**
         * @brief 将文件编译为语法树并输出
         */
        void out_grammar();

        void run();

        void out_token();

        /**
         * @brief trc的交互式终端界面
         */
        void tshell();

        void tdb();

        /**
         * @brief Trc命令行操作帮助文档
         */
        void help();
    }
}
}
