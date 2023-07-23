/**
 * 虚拟机内置函数定义处
 */

module;
#include <cstdio>
#include <cstdlib>
#include <obj_malloc.hpp>
export module built_in_func;
import TRE;
import TVM;
import TVM.memory;
import Error;
import objs_pool;
import trc_int;
import trc_string;
import trcdef;
import language;

export namespace trc::TVM_space {
namespace builtin {
    /**
     * @brief 计算长度
     */
    void LENGTH(int argc, TVM* vm) {
        if (argc != 1) {
            vm->error_report(error::ArgumentError,
                language::error::argumenterror, "len", "1");
        }
        auto firsts = (def::STRINGOBJ)vm->pop();
        vm->push(new types::trc_int((int)firsts->len()));
    }

    /**
     * @brief 输出帮助
     * @warning
     * 其实这只是个指引，真正的帮助需要查看文档
     */
    void HELP([[maybe_unused]] int argc, [[maybe_unused]] TVM* vm) {
        puts("\nWelcome to Tree!If this is your "
             "first time using Trcs,you "
             "can read the Doc to find help.");
    }

    /**
     * @brief 退出程序
     */
    void EXIT(int argc, TVM* vm) {
        exit(((def::INTOBJ)vm->pop())->value);
    }

    /**
     * @brief 打印argc个值
     */
    void PRINT(int argc, TVM* vm) {
        for (int i = 0; i < argc; ++i) {
            vm->pop()->putline(stdout);
        }
    }

    /**
     * @brief 打印argc个值并换行
     */
    void PRINTLN(int argc, TVM* vm) {
        for (int i = 0; i < argc; ++i) {
            vm->pop()->putline(stdout);
        }
        putchar('\n');
    }

    /**
     * @brief 获取一行输入作为string存入栈
     */
    void INPUT(int argc, TVM* vm) {
        for (int i = 0; i < argc; ++i) {
            ((def::STRINGOBJ)vm->pop())->putline(stdout);
        }
        def::STRINGOBJ strtmp = MALLOCSTRING();
        strtmp->in(stdin);
        vm->push(strtmp);
    }

    /**
     * @brief 强制转化为int类型
     */
    void INT_(int argc, TVM* vm) {
        vm->top() = vm->top()->to_int();
    }

    /**
     * 强制转化为float类型
     */
    void FLOAT_(int argc, TVM* vm) {
        // 利用了写好的底层转化函数，将不是double型的转化为double
        vm->top() = vm->top()->to_float();
    }

    /**
     * @brief 强制转化为string类型
     */
    void STRING_(int argc, TVM* vm) {
        // 利用了写好的底层转化函数，将不是string型的转化为string
        vm->top() = vm->top()->to_string();
    }

    /**
     * @brief 强制转化为布尔值
     */
    void BOOL_(int argc, TVM* vm) {
        vm->top() = vm->top()->to_bool();
    }

    /**
     * @brief 获取数据类型
     */
    void TYPE(int argc, TVM* vm) {
        vm->top()
            = MALLOCSTRING(TVM_share::int_name[(int)vm->top()->gettype()]);
    }
}
}
