/**
 * 编译器测试
 * 之所以直接跳到编译器测试而不是grammar测试是因为语法树不确定性太多，测试难度过大
 */

#include <Compiler/Compiler.h>
#include <TVM/TVM.h>
#include <TVM/memory.h>
#include <base/utils/data.hpp>
#include <gtest/gtest.h>
#include <vector>

using namespace trc::TVM_space;
using namespace trc;

/**
 * @brief 检查字节码是否匹配得上
 * @param expect 期待的字节码
 * @param vm 虚拟机
 */
static void bytecode_check(
    const TVM_space::struct_codes& expect, TVM_space::TVM* vm) {
    ASSERT_EQ(expect.size(), vm->static_data.byte_codes.size());
    for (size_t i = 0, n = expect.size(); i < n; ++i) {
        EXPECT_EQ(expect[i].bycode, vm->static_data.byte_codes[i].bycode);
        EXPECT_EQ(expect[i].index, vm->static_data.byte_codes[i].index);
    }
}

/*测试变量赋值解析*/
TEST(compiler, var) {
    TVM* vm = create_TVM();
    // 测试缓冲区内整形赋值
    compiler::Compiler(vm, "a:=80");
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[1], 80);
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    free_TVM(vm);
    // 测试缓冲区外整形赋值
    compiler::Compiler(vm, "a:=900");
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[1], 900);
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    free_TVM(vm);
    // 测试字符串赋值
    compiler::Compiler(vm, "a:=\"ppp\"");
    ASSERT_EQ(vm->static_data.const_s.size(), 2);
    EXPECT_STREQ(vm->static_data.const_s[1], "ppp");
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_STRING_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    free_TVM(vm);
    // 测试浮点型赋值
    compiler::Compiler(vm, "a:=9.08");
    ASSERT_EQ(vm->static_data.const_f.size(), 2);
    EXPECT_TRUE(utils::isequal(vm->static_data.const_f[1], 9.08))
        << vm->static_data.const_f[1];
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_FLOAT_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    free_TVM(vm);
    // 测试长整型赋值
    compiler::Compiler(vm, "a:=9999999999999999999");
    ASSERT_EQ(vm->static_data.const_long.size(), 2);
    EXPECT_STREQ(vm->static_data.const_long[1], "9999999999999999999");
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_LONG_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    delete vm;
}

// 测试函数调用的解析
TEST(compiler, function_call) {
    TVM* vm = create_TVM();
    // 测试内置变量的解析
    // 测试调用print函数
    // 不带变量
    compiler::Compiler(vm, "print(90)");
    ASSERT_EQ(vm->static_data.const_i.size(), 3);
    EXPECT_EQ(vm->static_data.const_i[1], 90);
    EXPECT_EQ(vm->static_data.const_i[2], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 2 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 2 } },
        vm);
    free_TVM(vm);
    // 带变量
    compiler::Compiler(vm, "a:=856\nprint(a)");
    // ASSERT_EQ(vm->static_data.const_i.size(), 3);
    // EXPECT_EQ(vm->static_data.const_i[1], 856);
    // EXPECT_EQ(vm->static_data.const_i[2], 1);
    // bytecode_check({{(bytecode_t)byteCodeNumber::LOAD_INT_, 1},
    // {(bytecode_t)byteCodeNumber::STORE_NAME_, 1},
    // {(bytecode_t)byteCodeNumber::LOAD_NAME_,
    // 1},{(bytecode_t)byteCodeNumber::LOAD_INT_, 2},
    // {(bytecode_t)byteCodeNumber::CALL_BUILTIN_, 2}},vm);
    free_TVM(vm);
    delete vm;
}

// 测试条件判断的解析
TEST(compiler, if_lex) {
}

// 测试while循环的解析
TEST(compiler, while_lex) {
}
