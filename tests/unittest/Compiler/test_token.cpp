/**
 * @file test_token.cpp
 * @brief token解析器的单元测试
 * @date 2022-05-03
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Compiler/compiler_def.hpp>
#include <Compiler/token.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace trc::compiler;

typedef std::vector<token> test_token_t;

class token_test_env : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;

    void check(const test_token_t& a) {
        token actual_data;
        /* 一个个检查是否匹配上了测试数据 */
        for (const auto& expect : a) {
            actual_data = lexer->get_token();
            ASSERT_NE(actual_data.tick, token_ticks::END_OF_TOKENS);
            EXPECT_EQ(expect.tick, actual_data.tick)
                << "expect:"<<(int)expect.tick << " actual:" << (int)actual_data.tick;
            EXPECT_EQ(expect.data, actual_data.data);
        }
        actual_data = lexer->get_token();
        EXPECT_EQ(actual_data.tick, token_ticks::END_OF_TOKENS);
    }

    /**
     * @brief 测试代码是否会输出相应的tokens
     *
     * @param test_code 测试代码
     * @param a 期望结果
     */
    void test_tokens(const std::string& test_code, const test_token_t& a) {
        lexer->set_code(test_code);
        check(a);
    }

    TVM_static_data vm {};

    compiler_public_data* compiler_data {};

    token_lex* lexer {};
};

void token_test_env::SetUp() {
    compiler_data = new compiler_public_data(
        "token_unittest", trc::compiler::nooptimize_option, vm);
    lexer = new token_lex(*compiler_data);
}

void token_test_env::TearDown() {
    delete compiler_data;
    delete lexer;
}

// 测试符号解析
TEST_F(token_test_env, oper_lex_1) {
    test_tokens("+", { { token_ticks::ADD, 0 } });
}

TEST_F(token_test_env, oper_lex_2) {
    test_tokens("2+ 1",
        { { token_ticks::INT_VALUE, 2 }, { token_ticks::ADD, 0 },
            { token_ticks::INT_VALUE, 1 } });
}

TEST_F(token_test_env, oper_lex_3) {
    test_tokens("1+4",
        { { token_ticks::INT_VALUE, 1 }, { token_ticks::ADD, 0 },
            { token_ticks::INT_VALUE, 2 } });
}

TEST_F(token_test_env, oper_lex_4) {
    test_tokens("1+1.0",
        { { token_ticks::INT_VALUE, 1 }, { token_ticks::ADD, 0 },
            { token_ticks::FLOAT_VALUE, 0 } });
}

TEST_F(token_test_env, oper_lex_5) {
    test_tokens("-+*/7.0+1-0",
        { { token_ticks::SUB, 0 }, { token_ticks::ADD, 0 },
            { token_ticks::MUL, 0 }, { token_ticks::DIV, 0 },
            { token_ticks::FLOAT_VALUE, 0 }, { token_ticks::ADD, 0 },
            { token_ticks::INT_VALUE, 1 }, { token_ticks::SUB, 0 },
            { token_ticks::INT_VALUE, 0 } });
}

// 测试条件表达式符号解析
TEST_F(token_test_env, condit_lex) {
    test_tokens("<1>!=>===1.8",
        { { token_ticks::LESS, 0 }, { token_ticks::INT_VALUE, 1 },
            { token_ticks::GREATER, 0 }, { token_ticks::UNEQUAL, 0 },
            { token_ticks::GREATER_EQUAL, 0 }, { token_ticks::EQUAL, 0 },
            { token_ticks::FLOAT_VALUE, 0 } });
}

// 等式赋值解析
TEST_F(token_test_env, as_lex_1) {
    test_tokens("a:=1+1\nb:=2",
        { { { token_ticks::NAME, 0 }, { token_ticks::STORE, 0 },
            { token_ticks::INT_VALUE, 1 }, { token_ticks::ADD, 0 },
            { token_ticks::INT_VALUE, 1 }, { token_ticks::END_OF_LINE, 0 },
            { token_ticks::NAME, 1 }, { token_ticks::STORE, 0 },
            { token_ticks::INT_VALUE, 2 } } });
}

TEST_F(token_test_env, as_lex_2) {
    test_tokens("a=input()",
        { { { token_ticks::NAME, 0 }, { token_ticks::ASSIGN, 0 },
            { token_ticks::NAME, 1 }, { token_ticks::LEFT_SMALL_BRACE, 0 },
            { token_ticks::RIGHT_SMALL_BRACE, 0 } } });
}

// 测试括号解析
TEST_F(token_test_env, symbols_lex) {
    test_tokens("()({})()[][]{([])}",
        { { token_ticks::LEFT_SMALL_BRACE, 0 },
            { token_ticks::RIGHT_SMALL_BRACE, 0 },
            { token_ticks::LEFT_SMALL_BRACE, 0 },
            { token_ticks::LEFT_BIG_BRACE, 0 },
            { token_ticks::RIGHT_BIG_BRACE, 0 },
            { token_ticks::RIGHT_SMALL_BRACE, 0 },
            { token_ticks::LEFT_SMALL_BRACE, 0 },
            { token_ticks::RIGHT_SMALL_BRACE, 0 },
            { token_ticks::LEFT_MID_BRACE, 0 },
            { token_ticks::RIGHT_MID_BRACE, 0 },
            { token_ticks::LEFT_MID_BRACE, 0 },
            { token_ticks::RIGHT_MID_BRACE, 0 },
            { token_ticks::LEFT_BIG_BRACE, 0 },
            { token_ticks::LEFT_SMALL_BRACE, 0 },
            { token_ticks::LEFT_MID_BRACE, 0 },
            { token_ticks::RIGHT_MID_BRACE, 0 },
            { token_ticks::RIGHT_SMALL_BRACE, 0 },
            { token_ticks::RIGHT_BIG_BRACE, 0 } });
}

// 测试字符串解析
TEST_F(token_test_env, string_lex_1) {
    test_tokens(R"("r\n\n\npopop")", { { token_ticks::STRING_VALUE, 0 } });
    EXPECT_EQ(vm.const_s[0], "r\n\n\npopop");
}

TEST_F(token_test_env, string_lex_2) {
    test_tokens(R"("o\n\t\n\t\tup")", { { token_ticks::STRING_VALUE, 0 } });
    EXPECT_EQ(vm.const_s[0], "o\n\t\n\t\tup");
}

TEST_F(token_test_env, string_lex_3) {
    test_tokens(R"('789\n712\t')", { { token_ticks::STRING_VALUE, 0 } });
    EXPECT_EQ(vm.const_s[0], "789\n712\t");
}

TEST_F(token_test_env, string_lex_4) {
    test_tokens(R"('\'')", { { token_ticks::STRING_VALUE, 0 } });
    EXPECT_EQ(vm.const_s[0], "'");
}

TEST_F(token_test_env, string_lex_5) {
    test_tokens(R"("\"")", { { token_ticks::STRING_VALUE, 0 } });
    EXPECT_EQ(vm.const_s[0], "\"");
}

// 测试函数调用解析
TEST_F(token_test_env, call_func_lex_1) {
    test_tokens("print(10)",
        { { token_ticks::NAME, 0 }, { token_ticks::LEFT_SMALL_BRACE, 0 },
            { token_ticks::INT_VALUE, 2 },
            { token_ticks::RIGHT_SMALL_BRACE, 0 } });
}

TEST_F(token_test_env, call_func_lex_2) {
    test_tokens(R"(print("p\np"))",
        { { token_ticks::NAME, 0 }, { token_ticks::LEFT_SMALL_BRACE, 0 },
            { token_ticks::STRING_VALUE, 0 },
            { token_ticks::RIGHT_SMALL_BRACE, 0 } });
}

// 测试数字解析
TEST_F(token_test_env, float_int_lex_1) {
    test_tokens("1.90+3.98-1",
        { { token_ticks::FLOAT_VALUE, 0 }, { token_ticks::ADD, 0 },
            { token_ticks::FLOAT_VALUE, 1 }, { token_ticks::SUB, 0 },
            { token_ticks::INT_VALUE, 1 } });
}

TEST_F(token_test_env, float_int_lex_2) {
    test_tokens("1.90001", { { token_ticks::FLOAT_VALUE, 0 } });
    EXPECT_EQ(vm.const_f[0], 1.90001);
}

TEST_F(token_test_env, float_int_lex_3) {
    test_tokens("1_2_3", { { token_ticks::INT_VALUE, 2 } });
    EXPECT_EQ(vm.const_i[2], 123);
}

TEST_F(token_test_env, float_int_lex_4) {
    test_tokens("1_2_4._23_1+1_",
        { { token_ticks::FLOAT_VALUE, 0 }, { token_ticks::ADD, 0 },
            { token_ticks::INT_VALUE, 1 } });
    EXPECT_EQ(vm.const_f[0], 124.231);
}

TEST_F(token_test_env, float_int_lex_5) {
    // 测试长整型解析
    test_tokens("11111111111111111111", { { token_ticks::LONG_INT_VALUE, 0 } });
    EXPECT_EQ(vm.const_long[0], "11111111111111111111");
}

// 测试英文字符解析
TEST_F(token_test_env, english) {
    // 命名标识符
    test_tokens("aaa a1 a_1_3_bc __a_c _a1c2_o r",
        { { token_ticks::NAME, 0 }, { token_ticks::NAME, 1 },
            { token_ticks::NAME, 2 }, { token_ticks::NAME, 3 },
            { token_ticks::NAME, 4 }, { token_ticks::NAME, 5 } });
    EXPECT_EQ(lexer->compiler_data.const_name.ref[0], "aaa");
    EXPECT_EQ(lexer->compiler_data.const_name.ref[1], "a1");
    EXPECT_EQ(lexer->compiler_data.const_name.ref[2], "a_1_3_bc");
    EXPECT_EQ(lexer->compiler_data.const_name.ref[3], "__a_c");
    EXPECT_EQ(lexer->compiler_data.const_name.ref[4], "_a1c2_o");
    EXPECT_EQ(lexer->compiler_data.const_name.ref[5], "r");
}

TEST_F(token_test_env, keyword) {
    // 保留字
    test_tokens("while for import goto func not or and",
        { { token_ticks::WHILE, 0 }, { token_ticks::FOR, 0 },
            { token_ticks::IMPORT, 0 }, { token_ticks::GOTO, 0 },
            { token_ticks::FUNC, 0 }, { token_ticks::NOT, 0 },
            { token_ticks::OR, 0 }, { token_ticks::AND, 0 } });
}

TEST_F(token_test_env, const_value) {
    // 特殊常量的解析
    test_tokens("null:=true+false",
        { { token_ticks::NULL_, 0 }, { token_ticks::STORE, 0 },
            { token_ticks::TRUE_, 0 }, { token_ticks::ADD, 0 },
            { token_ticks::FALSE_, 0 } });
}

// 测试if,while等语句
TEST_F(token_test_env, block_expr_lex) {
    test_tokens("if 1==1 {\n    print(1)\n}",
        { { token_ticks::IF, 0 }, { token_ticks::INT_VALUE, 1 },
            { token_ticks::EQUAL, 0 }, { token_ticks::INT_VALUE, 1 },
            { token_ticks::LEFT_BIG_BRACE, 0 }, { token_ticks::END_OF_LINE, 0 },
            { token_ticks::NAME, 0 }, { token_ticks::LEFT_SMALL_BRACE, 0 },
            { token_ticks::INT_VALUE, 1 },
            { token_ticks::RIGHT_SMALL_BRACE, 0 },
            { token_ticks::END_OF_LINE, 0 },
            { token_ticks::RIGHT_BIG_BRACE, 0 } });
}
