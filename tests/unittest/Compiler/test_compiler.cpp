/**
 * 编译器测试
 */

#include <Compiler/Compiler.h>
#include <TVM/TVM.h>
#include <gtest/gtest.h>
#include <vector>
using namespace trc;

/**
 * @brief 检查字节码是否匹配得上
 * @param expect
 * @param vm
 */
static void bytecode_check(
    const TVM_space::struct_codes& expect, TVM_space::TVM* vm) {

}

TEST(compiler, var) {
    TVM_space::TVM* vm = TVM_space::create_TVM();
    compiler::Compiler(vm, "a:=90");
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[1], 90);
    ASSERT_EQ(vm->static_data.const_name.size(), 2);
    EXPECT_STREQ(vm->static_data.const_name[1], "a");
    //bytecode_check({byteCodeNumber::LOAD_INT_, 1}, vm);
    delete vm;
}

TEST(compiler, if_lex) {
}
