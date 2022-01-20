#pragma once

namespace trc::TVM_space {
class TVM;
namespace builtin {
    /**
     * @brief 计算长度
     */
    void LENGTH(int argc, TVM* vm);

    /**
     * @brief 输出帮助
     * @warning
     * 其实这只是个指引，真正的帮助需要查看文档
     */
    void HELP(int argc, TVM* vm);

    /**
     * @brief 退出程序
     */
    void EXIT(int argc, TVM* vm);

    /**
     * @brief 打印argc个值
     */
    void PRINT(int argc, TVM* vm);

    /**
     * @brief 打印argc个值并换行
     */
    void PRINTLN(int argc, TVM* vm);

    /**
     * @brief 获取一行输入作为string存入栈
     */
    void INPUT(int argc, TVM* vm);

    /**
     * @brief 强制转化为int类型
     */
    void INT_(int argc, TVM* vm);

    /**
     * 强制转化为float类型
     */
    void FLOAT_(int argc, TVM* vm);

    /**
     * @brief 强制转化为string类型
     */
    void STRING_(int argc, TVM* vm);

    /**
     * @brief 强制转化为布尔值
     */
    void BOOL_(int argc, TVM* vm);

    /**
     * @brief 获取数据类型
     */
    void TYPE(int argc, TVM* vm);
}
}
