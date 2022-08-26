/**
 * 当编译为助记符后转化为对应的整数储存
 */


namespace trc::loader {
// 在反编译时把字节码转化为助记符
const char* int_code[] = { "LOAD_INT", "ADD", "NOP", "SUB", "MUL", "DIV",
    "GOTO", "STORE_NAME", "LOAD_NAME", "DEL", "LOAD_FLOAT", "LOAD_STRING",
    "CALL_BUILTIN", "IMPORT", "POW", "ZDIV", "MOD", "IF_FALSE_GOTO",
    "CHANGE_VALUE", "EQUAL", "UNEQUAL", "GREATER_EQUAL", "LESS_EQUAL", "LESS",
    "GREATER", "ASSERT", "NOT", "AND", "OR", "STORE_LOCAL", "LOAD_LOCAL",
    "CALL_FUNCTION", "FREE_FUNCTION", "CHANGE_LOCAL", "DEL_LOCAL", "LOAD_LONG",
    "LOAD_ARRAY", "CALL_METHOD", "LOAD_MAP" };
}
