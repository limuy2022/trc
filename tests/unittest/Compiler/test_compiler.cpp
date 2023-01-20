/**
 * 编译器测试
 */

#include <Compiler/Compiler.hpp>
#include <Compiler/compiler_def.hpp>
#include <TVM/TVM.hpp>
#include <TVM/memory.hpp>
#include <base/utils/data.hpp>
#include <gtest/gtest.h>
#include <vector>

using namespace trc::TVM_space;
using namespace trc;

class compiler_env_set : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;

    TVM* vm {};
};

void compiler_env_set::TearDown() {
    delete vm;
}

void compiler_env_set::SetUp() {
    vm = new TVM;
}

/**
 * @brief 检查字节码是否匹配得上
 * @param expect 期待的字节码
 * @param vm 虚拟机
 */
static void bytecode_check(
    const TVM_space::struct_codes& expect, TVM_space::TVM* vm) {
    for(auto i:expect) {
        std::cerr << (int)i.bycode<<' '<<i.index<<std::endl;
    }
    std::cerr <<"\n\n";
    for(auto i:vm->static_data.byte_codes) {
        std::cerr << (int)i.bycode<<' '<<i.index<<std::endl;
    }
    for (size_t i = 0, n = expect.size(); i < n; ++i) {
        EXPECT_EQ(expect[i].bycode, vm->static_data.byte_codes[i].bycode)
            << int(expect[i].bycode) << ' '
            << int(vm->static_data.byte_codes[i].bycode);
        EXPECT_EQ(expect[i].index, vm->static_data.byte_codes[i].index);
    }
    ASSERT_EQ(expect.size(), vm->static_data.byte_codes.size());
}

/*测试变量赋值解析*/
// 测试整形赋值
TEST_F(compiler_env_set, int) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("a:=80\nb:=900");
    ASSERT_EQ(vm->static_data.const_i.size(), 4);
    EXPECT_EQ(vm->static_data.const_i[2], 80);
    EXPECT_EQ(vm->static_data.const_i[3], 900);
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 2);
    bytecode_check(
        { { byteCodeNumber::LOAD_INT, 2 }, { byteCodeNumber::STORE_NAME, 0 },
            { byteCodeNumber::LOAD_INT, 3 },
            { byteCodeNumber::STORE_NAME, 1 } },
        vm);
}

// 测试字符串赋值
TEST_F(compiler_env_set, string) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("a:=\"ppp\"");
    ASSERT_EQ(vm->static_data.const_s.size(), 1);
    EXPECT_EQ(vm->static_data.const_s[0], "ppp");
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 1);
    bytecode_check({ { byteCodeNumber::LOAD_STRING, 0 },
                       { byteCodeNumber::STORE_NAME, 0 } },
        vm);
}

// 测试浮点型赋值
TEST_F(compiler_env_set, float) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("a:=9.08");
    ASSERT_EQ(vm->static_data.const_f.size(), 1);
    EXPECT_TRUE(utils::isequal(vm->static_data.const_f[0], 9.08));
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 1);
    bytecode_check({ { byteCodeNumber::LOAD_FLOAT, 0 },
                       { byteCodeNumber::STORE_NAME, 0 } },
        vm);
}

// 测试长整型赋值
TEST_F(compiler_env_set, long_int) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("a:=9999999999999999999");
    ASSERT_EQ(vm->static_data.const_long.size(), 1);
    EXPECT_EQ(vm->static_data.const_long[0], "9999999999999999999");
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 1);
    bytecode_check(
        { { byteCodeNumber::LOAD_LONG, 0 }, { byteCodeNumber::STORE_NAME, 0 } },
        vm);
}

// 测试函数调用的解析
// 测试内置变量的解析
TEST_F(compiler_env_set, builtin_without_var) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("print(90)");
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 90);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check(
        { { byteCodeNumber::LOAD_INT, 0 }, { byteCodeNumber::LOAD_INT, 1 },
            { byteCodeNumber::CALL_BUILTIN, 1 } },
        vm);
}

TEST_F(compiler_env_set, builtin_with_var) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("a:=856+1\nprint(a)");
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 857);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check(
        { { byteCodeNumber::LOAD_INT, 0 }, { byteCodeNumber::STORE_NAME, 0 },
            { byteCodeNumber::LOAD_NAME, 0 }, { byteCodeNumber::LOAD_INT, 1 },
            { byteCodeNumber::CALL_BUILTIN, 1 } },
        vm);
}

// 函数中嵌套着函数
TEST_F(compiler_env_set, func_in_func) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("a:=int(input())");
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 0);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check(
        { { byteCodeNumber::LOAD_INT, 0 }, { byteCodeNumber::CALL_BUILTIN, 3 },
            { byteCodeNumber::LOAD_INT, 1 },
            { byteCodeNumber::CALL_BUILTIN, 7 },
            { byteCodeNumber::STORE_NAME, 0 } },
        vm);
}

// 测试运算符表达式的解析
// 函数中包括运算符表达式
TEST_F(compiler_env_set, expr_in_func) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("print(1+1)");
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 2);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check(
        { { byteCodeNumber::LOAD_INT, 0 }, { byteCodeNumber::LOAD_INT, 1 },
            { byteCodeNumber::CALL_BUILTIN, 1 } },
        vm);
}

// 函数返回值与函数返回值相加
TEST_F(compiler_env_set, expr_with_func) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("print(int(input())+int(input()))");
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[0], 0);
    EXPECT_EQ(vm->static_data.const_i[1], 1);
    bytecode_check(
        { { byteCodeNumber::LOAD_INT, 0 }, { byteCodeNumber::CALL_BUILTIN, 3 },
            { byteCodeNumber::LOAD_INT, 1 },
            { byteCodeNumber::CALL_BUILTIN, 7 },
            { byteCodeNumber::LOAD_INT, 0 },
            { byteCodeNumber::CALL_BUILTIN, 3 },
            { byteCodeNumber::LOAD_INT, 1 },
            { byteCodeNumber::CALL_BUILTIN, 7 }, { byteCodeNumber::ADD, 0 },
            { byteCodeNumber::LOAD_INT, 1 },
            { byteCodeNumber::CALL_BUILTIN, 1 } },
        vm);
}

/*常量折叠单元测试开始*/

// 整形和整形
TEST_F(compiler_env_set, const_fold_opt_1) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("1+2*3");
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 7);
}

// 整形和浮点型
TEST_F(compiler_env_set, const_fold_opt_2) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("1.2*4");
    ASSERT_EQ(vm->static_data.const_f.size(), 1);
    EXPECT_EQ(vm->static_data.const_f[0], 4.8);
}

// 浮点型和整形
TEST_F(compiler_env_set, const_fold_opt_3) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("4*1.2");
    ASSERT_EQ(vm->static_data.const_f.size(), 1);
    EXPECT_EQ(vm->static_data.const_f[0], 4.8);
}

// 整形和字符串
TEST_F(compiler_env_set, const_fold_opt_4) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("3*\"abc123\"");
    ASSERT_EQ(vm->static_data.const_i.size(), 0);
    ASSERT_EQ(vm->static_data.const_s.size(), 1);
    EXPECT_EQ(vm->static_data.const_s[0], "abc123abc123abc123");
}

// 字符串和整形
TEST_F(compiler_env_set, const_fold_opt_5) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("\"abc123\"*3");
    ASSERT_EQ(vm->static_data.const_i.size(), 0);
    ASSERT_EQ(vm->static_data.const_s.size(), 1);
    EXPECT_EQ(vm->static_data.const_s[0], "abc123abc123abc123");
}

// 字符串和字符串
TEST_F(compiler_env_set, const_fold_opt_6) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile(R"("12"+"34")");
    ASSERT_EQ(vm->static_data.const_s.size(), 1);
    EXPECT_EQ(vm->static_data.const_s[0], "1234");
}

// 小括号
TEST_F(compiler_env_set, const_fold_opt_9) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile(R"((2+2)-(1*1))");
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 3);
}

// 复杂的综合测试
TEST_F(compiler_env_set, const_fold_opt_7) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile(R"("ab"*(10+(1-2)))");
    ASSERT_EQ(vm->static_data.const_s.size(), 1);
    ASSERT_EQ(vm->static_data.const_i.size(), 0);
    EXPECT_EQ(vm->static_data.const_s[0], "ababababababababab");
}

// 简单条件表达式折叠
TEST_F(compiler_env_set, const_fold_opt_10) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("1!=2");
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 1);
}

// 测试较复杂条件表达式的常量折叠
TEST_F(compiler_env_set, const_fold_opt_8) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile(R"((1==1)+(1!=2))");
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 2);
}

// 字符串条件表达式
TEST_F(compiler_env_set, const_fold_opt_11) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile(R"(("a">"c")+("a"=="ab")+("c"=='c'))");
    ASSERT_EQ(vm->static_data.const_s.size(), 0);
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 1);
}

// 不同类型条件表达式
TEST_F(compiler_env_set, const_fold_opt_12) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile(R"((1>0.2)+(1.2<3))");
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 2);
}

/*常量折叠单元测试结束*/

// 测试条件判断的解析
TEST_F(compiler_env_set, if_lex) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("if 1==1{\nprint(1)\n}");
    // 一个是跳转行号，一个是1
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 1);
    bytecode_check(
        { { byteCodeNumber::LOAD_INT, 0 }, { byteCodeNumber::IF_FALSE_GOTO, 5 },
            { byteCodeNumber::LOAD_INT, 0 }, { byteCodeNumber::LOAD_INT, 0 },
            { byteCodeNumber::CALL_BUILTIN, 1 } },
        vm);
}

// 测试while循环
TEST_F(compiler_env_set, while_lex) {
    compiler::Compiler(
        compiler::main_module, compiler::nooptimize_option, vm->static_data)
        .compile("while 1==1{\nprint(1)\n}");
    ASSERT_EQ(vm->static_data.const_i.size(), 1);
    EXPECT_EQ(vm->static_data.const_i[0], 1);
    bytecode_check(
        { { byteCodeNumber::LOAD_INT, 0 }, { byteCodeNumber::IF_FALSE_GOTO, 6 },
            { byteCodeNumber::LOAD_INT, 0 }, { byteCodeNumber::LOAD_INT, 0 },
            { byteCodeNumber::CALL_BUILTIN, 1 }, { byteCodeNumber::GOTO, 0 } },
        vm);
}
