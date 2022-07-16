/**
 * @file pri_compiler.hpp
 * @brief Compiler模块的私有成员
 * @date 2022-01-18
 *
 * @copyright Copyright (c) 2022
 */

#pragma once

#include <Compiler/library.h>
#include <array>
#include <base/Error.h>
#include <base/trcdef.h>
#include <base/utils/data.hpp>
#include <cctype>
#include <list>
#include <string>

namespace trc::compiler {
// token的标识，避免重复解析
enum class token_ticks {
    FOR, // for
    WHILE, // while
    IF, // if
    FUNC, // function
    CLASS, // class
    ADD, // +
    SUB, // -
    MUL, // *
    DIV, // /
    ZDIV, // //
    MOD, // %
    POW, // **
    AND, // and
    OR, // or
    NOT, // not
    EQUAL, // ==
    UNEQUAL, // !=
    LESS, // <
    GREATER, // >
    LESS_EQUAL, // <=
    GREATER_EQUAL, // >=
    IMPORT, // import
    GOTO, // goto
    DEL, // del
    BREAK, // break
    CONTINUE, // continue
    ASSERT, // assert
    SELFADD, // +=
    SELFSUB, // -=
    SELFMUL, // *=
    SELFDIV, // /=
    SELFZDIV, // //=
    SELFMOD, // %=
    SELFPOW, // **=
    ASSIGN, // =
    STORE, // :=
    NAME, // 名称
    NULL_, // null
    TRUE, // true
    FALSE, // false
    STRING_VALUE, // 字符串值
    LONG_FLOAT_VALUE, //长浮点型值
    FLOAT_VALUE, // 浮点数值
    LONG_INT_VALUE, //长整型值
    INT_VALUE, // 整型值
    LEFT_BIG_BRACE, // {
    RIGHT_BIG_BRACE, // }
    LEFT_SMALL_BRACE, // (
    RIGHT_SMALL_BRACE, // )
    LEFT_MID_BRACE, // [
    RIGHT_MID_BRACE, // ]
    POINT, //.
    COMMA, // ,
    UNKNOWN, // unknown value,可以暂时用来占个位
    END_OF_TOKENS, // 解析结束
    END_OF_LINE, //行结束
};

/**
 * @brief token
 * @details 一个完整的token包括标识和值两部分,是解析器的基本单元
 */
class TRC_Compiler_api token {
public:
    // 标识
    token_ticks tick;
    // 值
    char* data;

    token(token_ticks, const char* data, size_t len);

    token(token_ticks);

    token() = default;

    /**
     * @brief 设置字符串大小
     * @param len 字符串长度(不包括\0)
     */
    void set_size(size_t len);

    ~token();
};

extern std::array<std::string, 7> opti_opers;
extern std::array<std::string, 8> opti_condits;
extern std::array<std::string, 3> const_values;
extern std::map<std::string, int> change_const;

// 语法生成树中的标识
enum class grammar_type {
    // 纯粹数据
    DATA,
    // 生成无参数字节码
    OPCODE,
    // 内置函数
    BUILTIN_FUNC,
    // 树
    TREE,
    // 带有参数的字节码,分两种情况，参数为子节点或者参数为节点自带的数据
    OPCODE_ARGV,
    // 函数定义，由于字节码十分特殊
    FUNC_DEFINE,
    // 变量定义，由于变量作用域特殊
    VAR_DEFINE,
    // 调用自定义函数
    CALL_FUNC,
    // 整型节点
    NUMBER,
    // 浮点型节点
    FLOAT,
    // 长整型节点
    LONG_INT,
    // 长浮点型节点
    LONG_FLOAT,
    //字符串型节点
    STRING,
    // 变量名节点
    VAR_NAME,
    // 描述一件表达式或者运算符表达式
    EXPR
};

/**
 * 树节点的基类，生成语法分析树时用到
 * 从中派生出两个不同的子类，一个用于存放数据，另一个仅仅存放token_ticks，通过基类进行类型擦除
 */
class TRC_Compiler_api treenode {
public:
    virtual ~treenode() = 0;

    virtual bool has_son() = 0;

    grammar_type type = grammar_type::TREE;
};

/**
 * @brief
 * 描述不是终结节点的类(不储存数据的非叶子节点)
 */
class TRC_Compiler_api is_not_end_node : public treenode {
public:
    is_not_end_node(grammar_type type);

    is_not_end_node() = default;

    std::list<treenode*> son;

    void connect(treenode* son_node);

    bool has_son() override;
};

/**
 * @brief 描述终结节点
 */
class TRC_Compiler_api is_end_node : public treenode {
public:
    bool has_son() override;
};

/**
 * @brief 数据节点
 */
class TRC_Compiler_api data_node {
public:
    char* data = nullptr;

    data_node(const std::string& data);

    data_node(const char* data);

    void set(const std::string&);

    void set(const char*);

    data_node() = default;

    virtual ~data_node();

    /**
     * @brief 交换token对象中的const
     * char*给此节点，只进行对象所有权的转移而消除了多次申请，释放和拷贝内存
     * @param token_value
     */
    void swap_string_data(token* token_value);

private:
    /**
     * @brief 重新设置大小
     * @warning 包括\0在内，参数不需要把\0考虑进来
     */
    void set_alloc(size_t sizes);
};

class TRC_Compiler_api tick_node {
public:
    token_ticks tick;

    explicit tick_node(token_ticks tick);
};

/**
 * @brief 基于token符号的节点
 */
class TRC_Compiler_api node_base_tick : public is_not_end_node,
                                        public tick_node {
public:
    node_base_tick(grammar_type type, token_ticks tick);
};

/**
 * @brief 基于数据的节点
 */
class TRC_Compiler_api node_base_data : public is_not_end_node,
                                        public data_node {
public:
    node_base_data(grammar_type type_argv, const char* data);

    explicit node_base_data(grammar_type type, token* data);

    explicit node_base_data(grammar_type type = grammar_type::TREE);
};

/**
 * @brief 基于标签的没有子节点的类型
 */
class TRC_Compiler_api node_base_tick_without_sons : public is_end_node,
                                                     public tick_node {
public:
    node_base_tick_without_sons(grammar_type type, token_ticks);
};

/**
 * @brief 基于字符串的没有子节点的节点
 */
class TRC_Compiler_api node_base_data_without_sons : public data_node,
                                                     public is_end_node {
public:
    node_base_data_without_sons(grammar_type type, const char* name);

    explicit node_base_data_without_sons(grammar_type type);

    explicit node_base_data_without_sons(grammar_type type, token* data);

    node_base_data_without_sons();
};

/**
 * @brief 基于字符串的没有子节点的节点
 */
class TRC_Compiler_api node_base_string_without_sons : public is_end_node,
                                                       public data_node {
public:
    node_base_string_without_sons(const char* data);

    explicit node_base_string_without_sons(token* data);
};

/**
 * @brief 基于整型的没有子节点的类型
 */
class TRC_Compiler_api node_base_int_without_sons : public is_end_node {
public:
    int value;

    explicit node_base_int_without_sons(
        int value, grammar_type type = grammar_type::NUMBER);
};

/**
 * @brief 基于整形的有子节点的类型
 */
class TRC_Compiler_api node_base_int : public is_not_end_node {
public:
    int value;

    explicit node_base_int(int value, grammar_type type = grammar_type::NUMBER);
};

/**
 * @brief 基于浮点型的没有子节点的类型
 */
class TRC_Compiler_api node_base_float_without_sons : public is_end_node {
public:
    double value;

    explicit node_base_float_without_sons(double value);
};

/**
 * @brief 判断是否为字母，包括_
 * @param c 字符
 */
inline bool is_english(char c) {
    return isalpha(c) || c == '_';
}

/**
 * @brief 适用于相邻的多个元素访问，使用方法同map
 * @details
 * 为什么不直接使用map？很简单，因为map在这里太笨重了,相当于一种优化
 * @tparam T 值的类型
 * @tparam index_t 索引的类型，因为可能是枚举类
 */
template <typename T, typename index_t> class next_order_map {
public:
    /**
     * @brief 访问元素
     * @param index 索引
     */
    T& operator[](index_t index) const;

    /**
     * @param map_ 数据表
     * @param begin 比较的基准
     */
    next_order_map(T* map_, index_t begin);

private:
    // 储存数据的表
    T* map_;

    // 比较的基准
    const size_t begin;
};

template <typename T, typename index_t>
T& next_order_map<T, index_t>::operator[](index_t index) const {
    return map_[(size_t)index - begin];
}

template <typename T, typename index_t>
next_order_map<T, index_t>::next_order_map(T* map_, index_t begin)
    : map_(map_)
    , begin((size_t)begin) {
}

// 用于获取运算符(条件运算符)优先级
extern const next_order_map<int, token_ticks> cal_priority;
// 用于对应grammar_type标识到字符串名称的表，便于报错
extern const next_order_map<const char*, grammar_type> str_grammar_type_cal_map;
// 用于对应token_ticks标识到字符串名称的表，便于报错
extern const next_order_map<const char*, token_ticks> str_token_ticks_cal_map;

/**
 * @brief 判断枚举值是否是可运算的
 */
inline bool is_cal_value(token_ticks tick) {
    return utils::inrange(token_ticks::NAME, token_ticks::INT_VALUE, tick);
}

/**
 * @brief 判断token是否是=或:=
 */
inline bool is_as_token(token_ticks tick) {
    return utils::inrange(token_ticks::ASSIGN, token_ticks::STORE, tick);
}

/**
 * @brief 判断是不是条件运算符token
 */
inline bool is_condit_token(token_ticks tick) {
    return utils::inrange(token_ticks::AND, token_ticks::GREATER_EQUAL, tick);
}

/**
 * @brief 判断是不是运算符token
 */
inline bool is_cal_token(token_ticks tick) {
    return utils::inrange(token_ticks::ADD, token_ticks::POW, tick);
}

/**
 * @brief 判断是不是语句token
 */
inline bool is_sentence_token(token_ticks tick) {
    return utils::inrange(token_ticks::IMPORT, token_ticks::ASSERT, tick);
}

/**
 * @brief 判断是不是带参数的语句token
 */
inline bool is_sentence_with_one_argv(token_ticks tick) {
    return utils::inrange(token_ticks::GOTO, token_ticks::GOTO, tick);
}

/**
 * @brief 判断是不是常量token
 */
inline bool is_const_value(token_ticks tick) {
    return utils::inrange(token_ticks::NULL_, token_ticks::FALSE, tick);
}

/**
 * @brief 判断是否为左值
 */
inline bool is_left_value(token_ticks tick) {
    return tick == token_ticks::NAME;
}

/**
 * @brief 判断是否为\n或\0符号
 */
inline bool is_end_token(token_ticks tick) {
    return tick == token_ticks::END_OF_TOKENS
        || tick == token_ticks::END_OF_LINE;
}

/**
 * @brief 判断是不是会引起语句块的节点
 */
inline bool is_blocked_token(token_ticks tick) {
    return utils::inrange(token_ticks::FOR, token_ticks::CLASS, tick);
}

/**
 * @brief 判断是不是非变量数据
 */
inline bool is_novar_data_token(token_ticks tick) {
    return utils::inrange(token_ticks::NULL_, token_ticks::INT_VALUE, tick);
}

/**
 * @brief 判断是不是数据节点
 */
inline bool is_no_var_data_node(grammar_type node_type) {
    return utils::inrange(
        grammar_type::NUMBER, grammar_type::STRING, node_type);
}

/**
 * @brief 判断是不是数字类型的节点，浮点数，整型，长整型等都算在内
 */
inline bool is_number_class_node(grammar_type node_type) {
    return utils::inrange(
        grammar_type::NUMBER, grammar_type::LONG_FLOAT, node_type);
}
}
