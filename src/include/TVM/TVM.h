/**
 * 虚拟机，执行TVM字节码的地方
 * 这里负责存放它的接口
 */

#pragma once

#include <TVM/TRE.h>
#include <TVM/TVM_data.h>
#include <base/utils/bytes.h>
#include <stack>
#include <string>
#include <vector>

enum class byteCodeNumber {
    LOAD_INT_,
    ADD_,
    SUB_,
    MUL_,
    DIV_,
    GOTO_,
    STORE_NAME_,
    LOAD_NAME_,
    LOAD_FLOAT_,
    LOAD_STRING_,
    CALL_BUILTIN_,
    IMPORT_,
    POW_,
    ZDIV_,
    MOD_,
    IF_FALSE_GOTO_,
    CHANGE_VALUE_,
    EQUAL_,
    UNEQUAL_,
    GREATER_EQUAL_,
    LESS_EQUAL_,
    LESS_,
    GREATER_,
    ASSERT_,
    NOT_,
    AND_,
    OR_,
    STORE_LOCAL_,
    CALL_FUNCTION_,
    FREE_FUNCTION_,
    LOAD_LOCAL_,
    CHANGE_LOCAL_,
    LOAD_LONG_,
    LOAD_ARRAY_,
    CALL_METHOD_,
    LOAD_MAP_,
    UNKNOWN
};

namespace trc::TVM_space {
/**
 * @brief trc的核心部分，负责执行字节码
 * @details 这是TVM的接口，实现在别的类
 */
class TVM {
public:
    TVM(std::string name = "__main__");

    ~TVM();
    /**
     * @brief 弹出栈顶值并返回
     */
    inline def::OBJ pop() {
        return *(dyna_data.stack_top_ptr--);
    }

    /**
     * @brief 重新加载数据(编译完之后需要执行)
     */
    void reload_data();

    /**
     * @brief 弹出栈顶值，但
     * @warning 不会析构对象
     * @details
     * 作为一个不被利用的值，也不能被析构，因为它可能是int或bool的缓存
     */
    void pop_value();

    /**
     * @brief 将对象入栈
     */
    inline void push(def::OBJ a) {
        *(++dyna_data.stack_top_ptr) = a;
    }

    inline def::OBJ& top() {
        return *(dyna_data.stack_top_ptr);
    }

    /**
     * @brief 从头开始，执行所有字节码
     */
    void run_all();

    /**
     * @brief 执行一行字节码
     */
    void run_line_bycode();

    /**
     * @brief 对模块报错的一层封装
     * @param error 异常名
     * @param ... 异常信息
     */
    void error_report(int error, ...);

    /**
     * @brief 执行一条字节码
     * @param bycode 字节码
     */
    void run_bycode(const TVM_bytecode* bycode);

    /**
     * @brief 执行当前字节码并指向下一条
     * @warning 这个函数不会检查是否越界
     */
    inline void run_one_bycode() {
        run_bycode(&static_data.byte_codes[run_index]);
        run_index++;
    }

    void run_func(const func_& function);

    TVM_dyna_data dyna_data;

    // 储存模块
    TVM** modules;

    // 需要导入模块的个数（当时没有被导入也算）
    size_t modules_num = 0;

    // 模块名
    std::string name;

    // 静态数据，编译时生成
    TVM_static_data static_data;

    // 执行到的字节码索引
    size_t run_index = 0;

private:
    // 指令集定义开始

    /**
     * @brief 加载整型变量到栈
     * @param index 常量池中的索引
     */
    void LOAD_INT(bytecode_index_t index);

    /**
     * @brief 加载浮点型变量到栈
     */
    void LOAD_FLOAT(bytecode_index_t index);

    /**
     * @brief 加载字符串变量到栈
     */
    void LOAD_STRING(bytecode_index_t index);

    /**
     * @brief 加法运算
     * @details 支持对象：int，float，string
     */
    void ADD();

    /**
     * @brief 减法运算
     * @details 支持对象：int， float
     */
    void SUB();

    /**
     * @brief 乘法运算
     * @details 支持对象：int，float，string
     */
    void MUL();

    /**
     * @brief 除法运算
     * @details 支持对象：int， float
     */
    void DIV();

    /**
     * @brief 整除运算
     * @details 支持对象：int， float
     */
    void ZDIV();

    /**
     * @brief 乘方运算
     * @details 支持对象：int， float
     */
    void POW();

    /**
     * @brief 模运算
     * @details 支持对象：int， float
     */
    void MOD();

    /**
     * @brief 跳转到行
     * @param index 行的索引（以0开始计数）
     */
    void GOTO(bytecode_index_t index);

    /**
     * @brief 新建变量
     */
    void STORE_NAME(bytecode_index_t index);

    /**
     * @brief 读取变量的值
     */
    void LOAD_NAME(bytecode_index_t index);

    /**
     * @brief 执行内置函数
     */
    void CALL_BUILTIN(bytecode_index_t name);

    /**
     * @brief 模块导入
     * @details 模块导入规则：
     * 首先检查是否为自定义模块
     * 其次检查是否为由系统原生支持的模块(由c++语言支持，定义在importlib动态链接库中)
     * 然后检查TVM/packages/self_support(由语言本身支持的模块)
     * 最后检查TVM/packages/other_support(由第三方支持的模块)
     */
    void IMPORT();

    /**
     * @brief 如果为否，跳转代码到栈顶索引处
     */
    void IF_FALSE_GOTO(bytecode_index_t index);

    /**
     * @brief 改变变量的值（全局）
     */
    void CHANGE_VALUE(bytecode_index_t index);

    /**
     * @brief
     * 判断相等并将结果压入栈，假设两端类型相同
     * @warning
     * 如果不相等，将会在编译期间进行强制转换，不能通过者将会报出类型错误
     */
    void EQUAL();

    /**
     * @brief 判断不相等并将结果压入栈
     */
    void UNEQUAL();

    /**
     * @brief 判断是否大于等于
     */
    void GREATER_EQUAL();

    /**
     * @brief 判断是否小于等于
     */
    void LESS_EQUAL();

    /**
     * @brief 判断是否小于
     */
    void LESS();

    /**
     * @brief 判断是否大于
     */
    void GREATER();

    /**
     * @brief 取反栈顶的值
     */
    void NOT();

    /**
     * @brief
     * 将栈顶的两个值取出并且判断做逻辑与运算
     */
    void AND();

    /**
     * @brief
     * 将栈顶的两个值取出并且判断做逻辑或运算
     */
    void OR();

    /**
     * @brief
     * 断言，判断表达式是否为假，为假则报出AssertError错误
     */
    void ASSERT();

    /**
     * @brief 生成函数局部变量
     */
    void STORE_LOCAL(bytecode_index_t name);

    /**
     * @brief 读取函数局部变量
     */
    void LOAD_LOCAL(bytecode_index_t name);

    /**
     * @brief 执行自定义函数
     * @details
     * 事实上是在虚拟机内创建帧，在帧中完成字节码运行,帧就相当于一个小环境
     */
    void CALL_FUNCTION(bytecode_index_t index);

    /**
     * @brief 还原到调用函数前的环境
     */
    void FREE_FUNCTION();

    /**
     * @brief 改变局部变量的值
     */
    void CHANGE_LOCAL(bytecode_index_t index);

    /**
     * @brief 加载大整数变量到栈
     */
    void LOAD_LONG(bytecode_index_t index);

    /**
     * @brief 执行方法
     * @param index
     */
    void CALL_METHOD(bytecode_index_t index);

    void LOAD_ARRAY(bytecode_index_t index);

    /**
     * @brief 加载map型变量到栈
     * @param argc 有几个参数
     */
    void LOAD_MAP(bytecode_index_t argc);

    // 指令集定义结束
};
}
