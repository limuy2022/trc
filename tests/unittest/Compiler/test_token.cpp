/**
 * @file test_token.cpp
 * @brief token解析器的单元测试
 * @date 2022-05-03
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Compiler/Compiler.h>
#include <Compiler/compiler_def.h>
#include <Compiler/token.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace trc::compiler;

static compiler_public_data compiler_data { std::string("token_unittest"),
    trc::compiler::nooptimize_option };

typedef std::vector<std::pair<token_ticks, const char*>> test_token_t;

/**
 * @brief 测试代码是否会输出相应的tokens
 *
 * @param test_code 测试代码
 * @param a 期望结果
 */
static void test_tokens(const std::string& test_code,
    const std::vector<std::pair<token_ticks, const char*>>& a) {
    auto* lex = new token_lex(test_code, compiler_data);
    token* tokendata;
    /* 一个个检查是否匹配上了测试数据 */
    for (const auto& iter : a) {
        tokendata = lex->get_token();
        ASSERT_NE(tokendata->tick, token_ticks::END_OF_TOKENS);
        EXPECT_EQ(iter.first, tokendata->tick)
            << (int)iter.first << ' ' << (int)tokendata->tick;
        EXPECT_STREQ(iter.second, tokendata->data);
        delete tokendata;
    }
    tokendata = lex->get_token();
    EXPECT_EQ(tokendata->tick, token_ticks::END_OF_TOKENS);
    delete lex;
    delete tokendata;
}

// 测试符号解析
TEST(token, oper_lex) {
    test_tokens("+", { { token_ticks::ADD, nullptr } });
    test_tokens("2+ 1",
        { { token_ticks::INT_VALUE, "2" }, { token_ticks::ADD, nullptr },
            { token_ticks::INT_VALUE, "1" } });
    test_tokens("1+4",
        { { token_ticks::INT_VALUE, "1" }, { token_ticks::ADD, nullptr },
            { token_ticks::INT_VALUE, "4" } });
    test_tokens("1+1.0",
        { { token_ticks::INT_VALUE, "1" }, { token_ticks::ADD, nullptr },
            { token_ticks::FLOAT_VALUE, "1.0" } });
    test_tokens("-+*/7.0+1-0",
        { { token_ticks::SUB, nullptr }, { token_ticks::ADD, nullptr },
            { token_ticks::MUL, nullptr }, { token_ticks::DIV, nullptr },
            { token_ticks::FLOAT_VALUE, "7.0" }, { token_ticks::ADD, nullptr },
            { token_ticks::INT_VALUE, "1" }, { token_ticks::SUB, nullptr },
            { token_ticks::INT_VALUE, "0" } });
}

// 测试条件表达式符号解析
TEST(token, condit_lex) {
    test_tokens("<1>!=>===1.8",
        { { token_ticks::LESS, nullptr }, { token_ticks::INT_VALUE, "1" },
            { token_ticks::GREATER, nullptr },
            { token_ticks::UNEQUAL, nullptr },
            { token_ticks::GREATER_EQUAL, nullptr },
            { token_ticks::EQUAL, nullptr },
            { token_ticks::FLOAT_VALUE, "1.8" } });
}

// 等式赋值解析
TEST(token, as_lex) {
    test_tokens("a:=1+1\nb:=2",
        { { { token_ticks::NAME, "a" }, { token_ticks::STORE, nullptr },
            { token_ticks::INT_VALUE, "1" }, { token_ticks::ADD, nullptr },
            { token_ticks::INT_VALUE, "1" },
            { token_ticks::END_OF_LINE, nullptr }, { token_ticks::NAME, "b" },
            { token_ticks::STORE, nullptr },
            { token_ticks::INT_VALUE, "2" } } });
    test_tokens("a=input()",
        { { { token_ticks::NAME, "a" }, { token_ticks::ASSIGN, nullptr },
            { token_ticks::NAME, "input" },
            { token_ticks::LEFT_SMALL_BRACE, nullptr },
            { token_ticks::RIGHT_SMALL_BRACE, nullptr } } });
}

// 测试括号解析
TEST(token, symbols_lex) {
    test_tokens("()({})()[][]{([])}",
        { { token_ticks::LEFT_SMALL_BRACE, nullptr },
            { token_ticks::RIGHT_SMALL_BRACE, nullptr },
            { token_ticks::LEFT_SMALL_BRACE, nullptr },
            { token_ticks::LEFT_BIG_BRACE, nullptr },
            { token_ticks::RIGHT_BIG_BRACE, nullptr },
            { token_ticks::RIGHT_SMALL_BRACE, nullptr },
            { token_ticks::LEFT_SMALL_BRACE, nullptr },
            { token_ticks::RIGHT_SMALL_BRACE, nullptr },
            { token_ticks::LEFT_MID_BRACE, nullptr },
            { token_ticks::RIGHT_MID_BRACE, nullptr },
            { token_ticks::LEFT_MID_BRACE, nullptr },
            { token_ticks::RIGHT_MID_BRACE, nullptr },
            { token_ticks::LEFT_BIG_BRACE, nullptr },
            { token_ticks::LEFT_SMALL_BRACE, nullptr },
            { token_ticks::LEFT_MID_BRACE, nullptr },
            { token_ticks::RIGHT_MID_BRACE, nullptr },
            { token_ticks::RIGHT_SMALL_BRACE, nullptr },
            { token_ticks::RIGHT_BIG_BRACE, nullptr } });
}

// 测试字符串解析
TEST(token, string_lex) {
    test_tokens(
        R"("r\n\n\npopop")", { { token_ticks::STRING_VALUE, "r\n\n\npopop" } });
    test_tokens(R"("o\n\t\n\t\tup")",
        { { token_ticks::STRING_VALUE, "o\n\t\n\t\tup" } });
    test_tokens(
        R"('789\n712\t')", { { token_ticks::STRING_VALUE, "789\n712\t" } });
    test_tokens(R"('\'')", { { token_ticks::STRING_VALUE, "'" } });
    test_tokens(R"("\"")", { { token_ticks::STRING_VALUE, "\"" } });
}

// 测试函数调用解析
TEST(token, call_func_lex) {
    test_tokens("print(10)",
        { { token_ticks::NAME, "print" },
            { token_ticks::LEFT_SMALL_BRACE, nullptr },
            { token_ticks::INT_VALUE, "10" },
            { token_ticks::RIGHT_SMALL_BRACE, nullptr } });
    test_tokens(R"(print("p\np"))",
        { { token_ticks::NAME, "print" },
            { token_ticks::LEFT_SMALL_BRACE, nullptr },
            { token_ticks::STRING_VALUE, "p\np" },
            { token_ticks::RIGHT_SMALL_BRACE, nullptr } });
}

// 测试数字解析
TEST(token, float_int_lex) {
    test_tokens("1.90+3.98-1",
        { { token_ticks::FLOAT_VALUE, "1.90" }, { token_ticks::ADD, nullptr },
            { token_ticks::FLOAT_VALUE, "3.98" }, { token_ticks::SUB, nullptr },
            { token_ticks::INT_VALUE, "1" } });
    test_tokens("1.90001", { { token_ticks::FLOAT_VALUE, "1.90001" } });
    test_tokens("1_2_3", { { token_ticks::INT_VALUE, "123" } });
    test_tokens("1_2_4._23_1+1_",
        { { token_ticks::FLOAT_VALUE, "124.231" },
            { token_ticks::ADD, nullptr }, { token_ticks::INT_VALUE, "1" } });
    // 测试长整型解析
    test_tokens("11111111111111111111",
        { { token_ticks::LONG_INT_VALUE, "11111111111111111111" } });
    // 测试长浮点型解析
    test_tokens("11111111111111111111.11",
        { { token_ticks::LONG_FLOAT_VALUE, "11111111111111111111.11" } });
}

// 测试英文字符解析
TEST(token, english) {
    // 命名标识符
    test_tokens("aaa a1 a_1_3_bc __a_c _a1c2_o r",
        { { token_ticks::NAME, "aaa" }, { token_ticks::NAME, "a1" },
            { token_ticks::NAME, "a_1_3_bc" }, { token_ticks::NAME, "__a_c" },
            { token_ticks::NAME, "_a1c2_o" }, { token_ticks::NAME, "r" } });
    // 保留字
    test_tokens("while for import goto func not or and",
        { { token_ticks::WHILE, nullptr }, { token_ticks::FOR, nullptr },
            { token_ticks::IMPORT, nullptr }, { token_ticks::GOTO, nullptr },
            { token_ticks::FUNC, nullptr }, { token_ticks::NOT, nullptr },
            { token_ticks::OR, nullptr }, { token_ticks::AND, nullptr } });
    // 特殊常量的解析
    test_tokens("null:=true+false",
        { { token_ticks::NULL_, nullptr }, { token_ticks::STORE, nullptr },
            { token_ticks::TRUE_, nullptr }, { token_ticks::ADD, nullptr },
            { token_ticks::FALSE_, nullptr } });
}

// 测试if,while等语句
TEST(token, block_expr_lex) {
    test_tokens("if 1==1 {\n    print(1)\n}",
        { { token_ticks::IF, nullptr }, { token_ticks::INT_VALUE, "1" },
            { token_ticks::EQUAL, nullptr }, { token_ticks::INT_VALUE, "1" },
            { token_ticks::LEFT_BIG_BRACE, nullptr },
            { token_ticks::END_OF_LINE, nullptr },
            { token_ticks::NAME, "print" },
            { token_ticks::LEFT_SMALL_BRACE, nullptr },
            { token_ticks::INT_VALUE, "1" },
            { token_ticks::RIGHT_SMALL_BRACE, nullptr },
            { token_ticks::END_OF_LINE, nullptr },
            { token_ticks::RIGHT_BIG_BRACE, nullptr } });
}

// 退回token的期望结果
test_token_t expr = { { token_ticks::INT_VALUE, "1" },
    { token_ticks::ADD, nullptr }, { token_ticks::FLOAT_VALUE, "1.0" } };
// 测试退回token的方法
TEST(token, unget_token) {
    auto* lex = new token_lex("1+1.0", compiler_data);
    token* tdata = lex->get_token();
    lex->unget_token(tdata);
    tdata = lex->get_token();
    lex->unget_token(tdata);
    for (int i = 0; i < 3; ++i) {
        tdata = lex->get_token();
        EXPECT_EQ(tdata->tick, expr[i].first) << (int)tdata->tick;
        EXPECT_STREQ(tdata->data, expr[i].second) << tdata->data;
        delete tdata;
    }
    delete lex;
}
