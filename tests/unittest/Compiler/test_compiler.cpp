/**
 * 编译器测试
 */

#include <Compiler/Compiler.h>
#include <TVM/TVM.h>
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

TEST(compiler, var) {
    TVM* vm = create_TVM();
    compiler::Compiler(vm, "a:=90");
    ASSERT_EQ(vm->static_data.const_i.size(), 2);
    EXPECT_EQ(vm->static_data.const_i[1], 90);
    ASSERT_EQ(vm->static_data.global_symbol_table_size, 1);
    bytecode_check({ { (bytecode_t)byteCodeNumber::LOAD_INT_, 1 },
                       { (bytecode_t)byteCodeNumber::STORE_NAME_, 0 } },
        vm);
    delete vm;
}

TEST(compiler, if_lex) {
}
