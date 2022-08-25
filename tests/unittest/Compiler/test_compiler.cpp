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

class compiler_env_set : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;

    TVM* vm;
};

void compiler_env_set::TearDown() {
    delete vm;
}

void compiler_env_set::SetUp() {
    vm = create_TVM();
}

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
TEST_F(compiler_env_set, int) {
    compiler::Compiler(vm, "a:=80\nb:=900", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 80);
    EXPECT_EQ(vm->static_data.const_i[1], 900);
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 1 } },
        vm);
}

// 测试字符串赋值
TEST_F(compiler_env_set, string) {
    compiler::Compiler(vm, "a:=\"ppp\"", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_s.size(), 1);
    EXPECT_STREQ(vm->static_data.const_s[0], "ppp");
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_STRING_, 0 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 0 } },
        vm);
}

// 测试浮点型赋值
TEST_F(compiler_env_set, float) {
    compiler::Compiler(vm, "a:=9.08", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_f.size(), 1);
    EXPECT_TRUE(utils::isequal(vm->static_data.const_f[0], 9.08));
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_FLOAT_, 0 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 0 } },
        vm);
}

// 测试长整型赋值
TEST_F(compiler_env_set, long_int) {
    compiler::Compiler(
        vm, "a:=9999999999999999999", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_long.size(), 1);
    EXPECT_STREQ(vm->static_data.const_long[0], "9999999999999999999");
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_LONG_, 0 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 0 } },
        vm);
}

// 测试带运算的赋值
TEST_F(compiler_env_set, assign_with_oper) {
    compiler::Compiler(vm, "a:=1+34\n", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 1);
    EXPECT_EQ(vm->static_data.const_i[1], 34);
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::ADD_, 0 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 0 } },
        vm);
}

// 测试函数调用的解析
// 测试内置变量的解析
TEST_F(compiler_env_set, builtin_without_var) {
    compiler::Compiler(vm, "print(90)", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 90);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 1 } },
        vm);
}

TEST_F(compiler_env_set, builtin_with_var) {
    compiler::Compiler(vm, "a:=856+1\nprint(a)", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 856);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::ADD_, 0 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_NAME_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 1 } },
        vm);
}

// 函数中嵌套着函数
TEST_F(compiler_env_set, func_in_func) {
    compiler::Compiler(vm, "a:=int(input())", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 0);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 3 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 7 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 0 } },
        vm);
}

// 测试运算符表达式的解析
// 简单常量相加
TEST_F(compiler_env_set, simple) {
    compiler::Compiler(vm, "1+4", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 1);
    EXPECT_EQ(vm->static_data.const_i[1], 4);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::ADD_, 0 } },
        vm);
}

// 函数中包括运算符表达式
TEST_F(compiler_env_set, expr_in_func) {
    compiler::Compiler(vm, "print(1+1)", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::ADD_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 1 } },
        vm);
}

// 函数返回值与函数返回值相加
TEST_F(compiler_env_set, expr_with_func) {
    compiler::Compiler(
        vm, "print(int(input())+int(input()))", &compiler::nooptimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 0);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 3 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 7 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 3 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 7 },
                       { (bytecode_t)byteCodeNumber::ADD_, 0 },
                       { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 1 } },
        vm);
}

// 优化类型相同的表达式代码
TEST_F(compiler_env_set, optimize_same_types) {
    compiler::Compiler(vm, "1+2*3", &compiler::optimize_option);
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 7);
}

// 优化类型不同的表达式代码
TEST_F(compiler_env_set, optimize_with_different_types) {
    compiler::Compiler(vm, "1.2*4", &compiler::optimize_option);
    ASSERT_EQ(vm->static_data.const_f.size(), 1);
    EXPECT_EQ(vm->static_data.const_f[0], 4.8);
}

// 测试条件判断的解析
TEST_F(compiler_env_set, if_lex) {
//     compiler::Compiler(
//         vm, "if 1==1{\nprint(1)\n}", &compiler::nooptimize_option);
//     ASSERT_EQ(vm->static_data.const_i.size(), 1);
//     EXPECT_EQ(vm->static_data.const_i[0], 1);
//     bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
//                        { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
//                        { (bytecode_t)byteCodeNumber::EQUAL_, 0},
//                        { (bytecode_t)byteCodeNumber::IF_FALSE_GOTO_, 7 },
//                        { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
//                        { (bytecode_t)byteCodeNumber::LOAD_INT_, 0 },
//                        { (bytecode_t)byteCodeNumber::CALL_BUILTIN_, 1 } },
//         vm);
}

// 测试while循环的解析
TEST_F(compiler_env_set, while_lex) {
}
