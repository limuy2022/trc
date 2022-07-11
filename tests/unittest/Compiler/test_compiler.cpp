/**
 * 编译器测试
 * 之所以直接跳到编译器测试而不是grammar测试是因为语法树不确定性太多，测试难度过大
 */

#include <Compiler/Compiler.h>
#include <Compiler/compiler_def.h>
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
        EXPECT_EQ(expect[i].bycode, vm->static_data.byte_codes[i].bycode)
            << int(expect[i].bycode) << ' '
            << int(vm->static_data.byte_codes[i].bycode);
        EXPECT_EQ(expect[i].index, vm->static_data.byte_codes[i].index);
    }
}

/*测试变量赋值解析*/
// 测试整形赋值
TEST(var_assign, int) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "a:=80\nb:=900", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 3);
    EXPECT_EQ(vm->static_data.const_i[1], 80);
    EXPECT_EQ(vm->static_data.const_i[2], 900);
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 3);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 2 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 2 } },
        vm);
    delete vm;
}

// 测试字符串赋值
TEST(var_assign, string) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "a:=\"ppp\"", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_s.size(), 2);
    EXPECT_STREQ(vm->static_data.const_s[1], "ppp");
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_STRING_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    delete vm;
}

// 测试浮点型赋值
TEST(var_assign, float) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "a:=9.08", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_f.size(), 2);
    EXPECT_TRUE(utils::isequal(vm->static_data.const_f[1], 9.08));
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_FLOAT_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    delete vm;
}

// 测试长整型赋值
TEST(var_assign, long_int) {
    TVM* vm = create_TVM();
    compiler::Compiler(
        vm, "a:=9999999999999999999", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_long.size(), 2);
    EXPECT_STREQ(vm->static_data.const_long[1], "9999999999999999999");
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_LONG_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    delete vm;
}

// 测试带运算的赋值
TEST(var_assign, assign_with_oper) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "a:=1+34\n", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 3);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    EXPECT_EQ(vm->static_data.const_i[2], 34);
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 2 },
                       { (bytecode_t)byteCodeNumber::ADD_, 0 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    delete vm;
}

// 测试带函数的赋值
TEST(var_assign, assign_with_func) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "a:=int(input())", &compiler::nooptimize_option);
    delete vm;
}

// 测试函数调用的解析
// 测试内置变量的解析
TEST(function_call, builtin) {
    TVM* vm = create_TVM();
    // 测试调用print函数
    // 不带变量
    compiler::Compiler(vm, "print(90)", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 3);
    EXPECT_EQ(vm->static_data.const_i[1], 90);
    EXPECT_EQ(vm->static_data.const_i[2], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 2 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 2 } },
        vm);
    free_TVM(vm);
    // 带变量
    compiler::Compiler(vm, "a:=856+1\nprint(a)", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 3);
    EXPECT_EQ(vm->static_data.const_i[1], 856);
    EXPECT_EQ(vm->static_data.const_i[2], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 2 },
                       { (bytecode_t)byteCodeNumber::ADD_, 0 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_NAME_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 2 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 2 } },
        vm);
    free_TVM(vm);
    delete vm;
}

// 函数中嵌套着函数
TEST(function_call, func_in_func) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "a:=int(input())", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 3);
    EXPECT_EQ(vm->static_data.const_i[1], 0);
    EXPECT_EQ(vm->static_data.const_i[2], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 4 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 2 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 8 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
    delete vm;
}

// 测试运算符表达式的解析
// 简单常量相加
TEST(oper, simple) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "1+4", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 3) << vm->static_data.const_i[1];
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    EXPECT_EQ(vm->static_data.const_i[2], 4);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 2 },
                       { (bytecode_t)byteCodeNumber::ADD_, 0 } },
        vm);
    delete vm;
}

// 函数中包括运算符表达式
TEST(oper, expr_in_func) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "print(1+1)", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::ADD_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 2 } },
        vm);
    delete vm;
}

// 函数返回值与函数返回值相加
TEST(oper, expr_with_func) {
    TVM* vm = create_TVM();
    compiler::Compiler(
        vm, "print(int(input())+int(input()))", &compiler::nooptimize_option);
    delete vm;
}

// 优化相同的表达式代码
TEST(oper, optimize_same_types) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "1+2*3", &compiler::optimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[1], 7);
    delete vm;
}

// 类型不同的表达式代码优化
TEST(oper, optimize_with_different_types) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "1.2*4", &compiler::optimize_option);
    ASSERT_EQ(vm->static_data.const_f.size(), 2);
    EXPECT_EQ(vm->static_data.const_f[1], 4.8);
    delete vm;
}

// 测试条件判断的解析
TEST(compiler, if_lex) {
    TVM* vm = create_TVM();
    compiler::Compiler(
        vm, "if 1==1{\nprint(1)\n}", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::EQUAL_ },
                       { (bytecode_t)byteCodeNumber::IF_FALSE_GOTO_, 7 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 2 } },
        vm);
    free_TVM(vm);
    delete vm;
}

// 测试while循环的解析
TEST(compiler, while_lex) {
}
