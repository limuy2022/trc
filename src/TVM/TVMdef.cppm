/**
 * TVM中的一些的定义
 */

module;
#include <cstdint>
#include <cstdlib>
#include <vector>
export module TVMdef;
import trcdef;

namespace trc::TVM_space {
class TVM;

// 字节码的类型
export typedef def::byte_t bytecode_t;
// 字节码索引的类型
export typedef uint16_t bytecode_index_t;

// 运行时类型标识
export enum class RUN_TYPE_TICK {
    int_T,
    string_T,
    float_T,
    trc_long_T,
    trc_flong_T,
    map_T,
    array_T,
};

export enum class byteCodeNumber : bytecode_t {
    LOAD_INT,
    ADD,
    SUB,
    MUL,
    DIV,
    GOTO,
    STORE_NAME,
    LOAD_NAME,
    LOAD_FLOAT,
    LOAD_STRING,
    CALL_BUILTIN,
    IMPORT,
    POW,
    ZDIV,
    MOD,
    IF_FALSE_GOTO,
    CHANGE_VALUE,
    EQUAL,
    UNEQUAL,
    GREATER_EQUAL,
    LESS_EQUAL,
    LESS,
    GREATER,
    ASSERT,
    NOT,
    AND,
    OR,
    STORE_LOCAL,
    CALL_FUNCTION,
    FREE_FUNCTION,
    LOAD_LOCAL,
    CHANGE_LOCAL,
    LOAD_LONG,
    LOAD_ARRAY,
    CALL_METHOD,
    LOAD_MAP,
    UNKNOWN
};

export class TVM_bytecode {
public:
    TVM_bytecode(byteCodeNumber bycode, bytecode_index_t index)
        : bycode(bycode)
        , index(index) {
    }
    byteCodeNumber bycode;
    bytecode_index_t index;
};

/**
 * @brief 符号表，保存变量信息
 * 就是凭借这个结构优化变量访问速度为O(1)
 */
export class symbol_form {
public:
    def::OBJ* vars = nullptr;
    void reset(size_t size);
    explicit symbol_form(size_t size);
    symbol_form() = default;
    ~symbol_form();
};

symbol_form::~symbol_form() {
    free(vars);
}

symbol_form::symbol_form(size_t size)
    : vars((def::OBJ*)malloc(sizeof(def::OBJ) * size)) {
}

void symbol_form::reset(size_t size) {
    vars = (def::OBJ*)(realloc(vars, size * sizeof(def::OBJ)));
}

// 储存所有字节码的类型
export typedef std::vector<TVM_bytecode> struct_codes;
}
