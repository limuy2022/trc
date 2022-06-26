/**
 * @file test_token.cpp
 * @brief token解析器的单元测试
 * @date 2022-05-03
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Compiler/Compiler.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace trc::compiler;

static compiler_error error("token_unittest");

/**
 * @brief 测试代码是否会输出相应的tokens
 *
 * @param test_code 测试代码
 * @param a 期望结果
 */
static void test_tokens(
    const std::string& test_code, const std::vector<token>& a) {
    auto* lex = new token_lex((test_code), &error);
    token* tokendata;
    /* 一个个检查是否匹配上了测试数据 */
    for (const auto& iter : a) {
        tokendata = lex->get_token();
        ASSERT_NE(tokendata->tick, token_ticks::END_OF_TOKENS);
        EXPECT_EQ(iter.tick, tokendata->tick);
        EXPECT_EQ(iter.data, tokendata->data);
        delete tokendata;
    }
    tokendata = lex->get_token();
    EXPECT_EQ(tokendata->tick, token_ticks::END_OF_TOKENS);
    delete lex;
    delete tokendata;
}

// 测试符号解析
TEST(token, oper_lex) {
    test_tokens("+", { { token_ticks::ADD, "" } });
    test_tokens("2+ 1",
        { { token_ticks::INT_VALUE, "2" }, { token_ticks::ADD, "" },
            { token_ticks::INT_VALUE, "1" } });
    test_tokens("-+*/7.0+1-0",
        { { token_ticks::SUB, "" }, { token_ticks::ADD, "" },
            { token_ticks::MUL, "" }, { token_ticks::DIV, "" },
            { token_ticks::FLOAT_VALUE, "7.0" }, { token_ticks::ADD, "" },
            { token_ticks::INT_VALUE, "1" }, { token_ticks::SUB, "" },
            { token_ticks::INT_VALUE, "0" } });
}

// 测试条件表达式符号解析
TEST(token, condit_lex) {
    test_tokens("<1>!=>===1.8",
        { { token_ticks::LESS, "" }, { token_ticks::INT_VALUE, "1" },
            { token_ticks::GREATER, "" }, { token_ticks::UNEQUAL, "" },
            { token_ticks::GREATER_EQUAL, "" }, { token_ticks::EQUAL, "" },
            { token_ticks::FLOAT_VALUE, "1.8" } });
}

// 测试符号解析
TEST(token, symbols_lex) {
    // 测试括号解析
    test_tokens("()({})()[][]{([])}",
        {
            { token_ticks::LEFT_SMALL_BRACE, "" },
            { token_ticks::RIGHT_SMALL_BRACE, "" },
            { token_ticks::LEFT_SMALL_BRACE, "" },
            { token_ticks::LEFT_BIG_BRACE, "" },
            { token_ticks::RIGHT_BIG_BRACE, "" },
            { token_ticks::RIGHT_SMALL_BRACE, "" },
            { token_ticks::LEFT_SMALL_BRACE, "" },
            { token_ticks::RIGHT_SMALL_BRACE, "" },
            { token_ticks::LEFT_MID_BRACE, "" },
            { token_ticks::RIGHT_MID_BRACE, "" },
            { token_ticks::LEFT_MID_BRACE, "" },
            { token_ticks::RIGHT_MID_BRACE, "" },
            { token_ticks::LEFT_BIG_BRACE, "" },
            { token_ticks::LEFT_SMALL_BRACE, "" },
            { token_ticks::LEFT_MID_BRACE, "" },
            { token_ticks::RIGHT_MID_BRACE, "" },
            { token_ticks::RIGHT_SMALL_BRACE },
            { token_ticks::RIGHT_BIG_BRACE, "" },
        });
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
        { { token_ticks::NAME, "print" }, { token_ticks::LEFT_SMALL_BRACE, "" },
            { token_ticks::INT_VALUE, "10" },
            { token_ticks::RIGHT_SMALL_BRACE, "" } });
    test_tokens(R"(print("p\np"))",
        { { token_ticks::NAME, "print" }, { token_ticks::LEFT_SMALL_BRACE, "" },
            { token_ticks::STRING_VALUE, "p\np" },
            { token_ticks::RIGHT_SMALL_BRACE, "" } });
}

// 测试数字解析
TEST(token, float_int_lex) {
    test_tokens("1.90+3.98-1",
        { { token_ticks::FLOAT_VALUE, "1.90" }, { token_ticks::ADD, "" },
            { token_ticks::FLOAT_VALUE, "3.98" }, { token_ticks::SUB, "" },
            { token_ticks::INT_VALUE, "1" } });
    test_tokens("1.90001", { { token_ticks::FLOAT_VALUE, "1.90001" } });
    test_tokens("1_2_3", {{token_ticks::INT_VALUE, "123"}});
    test_tokens("1_2_4._23_1+1_", {{token_ticks::FLOAT_VALUE, "124.231"}, {token_ticks::ADD, ""}, {token_ticks::INT_VALUE,"1"}});
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
        { { token_ticks::WHILE, "" }, { token_ticks::FOR, "" },
            { token_ticks::IMPORT, "" }, { token_ticks::GOTO, "" },
            { token_ticks::FUNC, "" }, { token_ticks::NOT, "" },
            { token_ticks::OR, "" }, { token_ticks::AND, "" } });
}
