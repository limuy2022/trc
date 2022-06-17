/**
 * 字节码最终在此生成，是编译器的另一个核心
 */

#include <TVM/TVM_data.h>
#include <Compiler/Compiler.h>
#include <Compiler/grammar.h>
#include <Compiler/pri_compiler.hpp>
#include <TVM/TVMdef.h>
#include <TVM/func.h>
#include <base/Error.h>
#include <base/code_loader.h>
#include <base/utils/data.hpp>
#include <climits>
#include <cstring>
#include <language/error.h>
#include <string>
#include <vector>

namespace trc::compiler {
using TVM_space::bytecode_t;

// 将对应字符转化为字节码的过程，此过程仅能编译出助记符，并不能完全编译出字节码
static byteCodeNumber opcodesym_int[] = {
    byteCodeNumber::UNKNOWN, // for
    byteCodeNumber::IF_FALSE_GOTO_, // while
    byteCodeNumber::IF_FALSE_GOTO_, // if
    byteCodeNumber::UNKNOWN, // func
    byteCodeNumber::UNKNOWN, // class
    byteCodeNumber::ADD_,
    byteCodeNumber::SUB_,
    byteCodeNumber::MUL_,
    byteCodeNumber::DIV_,
    byteCodeNumber::ZDIV_,
    byteCodeNumber::MOD_,
    byteCodeNumber::POW_,
    byteCodeNumber::AND_,
    byteCodeNumber::OR_,
    byteCodeNumber::NOT_,
    byteCodeNumber::EQUAL_,
    byteCodeNumber::UNEQUAL_,
    byteCodeNumber::LESS_,
    byteCodeNumber::GREATER_,
    byteCodeNumber::LESS_EQUAL_,
    byteCodeNumber::GREATER_EQUAL_,
    byteCodeNumber::IMPORT_,
    byteCodeNumber::GOTO_,
    byteCodeNumber::DEL_,
    byteCodeNumber::ASSERT_,
};

class detail_compiler {
public:
    detail_compiler(compiler_error&, TVM_space::TVM_static_data&);

    /**
     * @brief 将一个节点解析成字节码
     * @param head 根节点
     * @param vm 需要编译的虚拟机
     */
    void compile(TVM_space::TVM* vm, treenode* head);

private:
    /**
     * @brief 把数据添加进相对应的vm常量池
     * @return 数据在常量池中所占的索引
     */
    static TVM_space::bytecode_index_t add(
        TVM_space::TVM* vm, COMPILE_TYPE_TICK data_type, char* data_value);

    /**
     * @brief
     * 构建字节码，以data在opcodesym_int中的值和index初始化数组
     * @param data 符号
     * @param index 索引
     */
    static TVM_space::TVM_bytecode* build_opcode(
        token_ticks symbol, TVM_space::bytecode_index_t index = 0);

    /**
     * @brief 构建有关变量的字节码
     * @param data 符号
     * @param index 索引
     */
    static TVM_space::TVM_bytecode* build_var(
        token_ticks data, TVM_space::bytecode_index_t index = 0);

    /**
     * @brief 添加字节码，并完善对应环境
     */
    void add_opcode(TVM_space::TVM_bytecode*);

    /**
     * @brief 解析函数字节码
     */
    void func_lexer(TVM_space::TVM* vm, treenode* head);

    compiler_error& error_;

    TVM_space::TVM_static_data& static_data;

    long long prev_value = LLONG_MAX;

    // 行号转化为字节码每行的第一个字节码的索引
    std::vector<int> line_to_bycodeindex_table;
};

void detail_compiler::add_opcode(TVM_space::TVM_bytecode* opcode) {
    static_data.byte_codes.push_back(opcode);
    static_data.line_number_table.push_back(error_.line);
    if (error_.line != prev_value) {
        line_to_bycodeindex_table.resize(error_.line + 1);
        line_to_bycodeindex_table[error_.line]
            = static_data.line_number_table.size() - 1;
        prev_value = error_.line;
    }
}

void detail_compiler::func_lexer(TVM_space::TVM* vm, treenode* head) {
}

TVM_space::bytecode_index_t detail_compiler::add(
    TVM_space::TVM* vm, COMPILE_TYPE_TICK data_type, char* data_value) {
    switch (data_type) {
    case string_TICK: {
        int index = utils::str_check_in_i(data_value,
            vm->static_data.const_s.begin() + 1, vm->static_data.const_s.end());
        size_t size = vm->static_data.const_s.size();
        if (index != -1) {
            return index + 1;
        } else {
            char* copy = new char[strlen(data_value) + 1];
            strcpy(copy, data_value);
            vm->static_data.const_s.push_back(copy);
            return size;
        }
    }
    case int_TICK: {
        int afdata = atoi(data_value);
        size_t size = vm->static_data.const_i.size();
        int index = utils::check_in_i(afdata,
            vm->static_data.const_i.begin() + 1, vm->static_data.const_i.end());
        if (index != -1) {
            return index + 1;
        } else {
            vm->static_data.const_i.push_back(afdata);
            return size;
        }
    }
    case float_TICK: {
        double afdata = atof(data_value);
        size_t size = vm->static_data.const_f.size();
        int index = utils::check_in_i(afdata,
            vm->static_data.const_f.begin() + 1, vm->static_data.const_f.end());
        if (index != -1) {
            return index + 1;
        } else {
            vm->static_data.const_f.push_back(afdata);
            return size;
        }
    }
    case VAR_TICK: {
        size_t size = vm->static_data.const_name.size();
        int index = utils::str_check_in_i(data_value,
            vm->static_data.const_name.begin() + 1,
            vm->static_data.const_name.end());
        if (index != -1) {
            return index + 1;
        } else {
            char* copy = new char[strlen(data_value) + 1];
            strcpy(copy, data_value);
            vm->static_data.const_name.push_back(copy);
            return size;
        }
    }
    case LONG_TICK: {
        size_t size = vm->static_data.const_long.size();
        int index = utils::str_check_in_i(data_value,
            vm->static_data.const_long.begin() + 1,
            vm->static_data.const_long.end());
        if (index != -1) {
            return index + 1;
        } else {
            char* copy = new char[strlen(data_value) + 1];
            strcpy(copy, data_value);
            vm->static_data.const_long.push_back(copy);
            return size;
        }
    }
    case CONST_TICK: {
        int afdata = change_const[data_value];
        size_t size = vm->static_data.const_i.size();
        int index = utils::check_in_i(afdata,
            vm->static_data.const_i.begin() + 1, vm->static_data.const_i.end());
        if (index != -1) {
            return index + 1;
        } else {
            vm->static_data.const_i.push_back(afdata);
            return size;
        }
    }
    default: {
        NOREACH;
    }
    }
}

TVM_space::TVM_bytecode* detail_compiler::build_opcode(
    token_ticks symbol, TVM_space::bytecode_index_t index) {
    return new TVM_space::TVM_bytecode { (bytecode_t)opcodesym_int[(int)symbol],
        index };
}

TVM_space::TVM_bytecode* detail_compiler::build_var(
    token_ticks data, TVM_space::bytecode_index_t index) {
    if (data == token_ticks::ASSIGN)
        return new TVM_space::TVM_bytecode {
            (bytecode_t)byteCodeNumber::CHANGE_VALUE_, index
        };
    return new TVM_space::TVM_bytecode {
        (bytecode_t)byteCodeNumber::STORE_NAME_, index
    };
}

void detail_compiler::compile(TVM_space::TVM* vm, treenode* head) {
    grammar_type type = head->type;
    if (head->has_son()) {
        auto* root = (is_not_end_node*)head;
        switch (type) {
        case TREE: {
            // 不是数据和传参节点，确认为树
            for (auto i : root->son) {
                compile(vm, i);
            }
            break;
        }
        case BUILTIN_FUNC: {
            // 内置函数
            auto* code = (node_base_tick_without_sons*)root->son[0];
            auto* index_ = (node_base_int_without_sons*)root->son[1];
            add_opcode(build_opcode(code->tick, index_->value));
            break;
        }
        case OPCODE_ARGV: {
            // 带参数字节码
            // 参数
            auto* argv_ = (node_base_data_without_sons*)root->son[0];
            COMPILE_TYPE_TICK type_data = what_type(argv_->data);
            add_opcode(build_opcode(((node_base_tick*)root)->tick,
                add(vm, type_data, argv_->data)));
            break;
        }
        case FUNC_DEFINE: {
            // 函数定义
            func_lexer(vm, head);
            break;
        }
        case VAR_DEFINE: {
            // 变量定义
            // 处理等式右边的数据
            compile(vm, root->son[1]);
            // 处理等式左边的数据
            auto* argv_ = (node_base_data_without_sons*)root->son[0];
            short index_argv
                = add(vm, COMPILE_TYPE_TICK::VAR_TICK, argv_->data);
            add_opcode(build_var(((node_base_tick*)root)->tick, index_argv));
            break;
        }
        case CALL_FUNC: {
            // 调用自定义函数
            // 判断函数是否存在
            char* nodedata = ((node_base_data*)root)->data;
            if (!utils::map_check_in_first(vm->static_data.funcs, nodedata))
                error_.send_error_module(
                    error::NameError, language::error::nameerror, nodedata);
            auto* code = (node_base_tick_without_sons*)root->son[0];
            auto* index_ = (node_base_int_without_sons*)root->son[1];
            add_opcode(build_opcode(code->tick, index_->value));
            break;
        }
        default: {
            NOREACH;
        }
        }
    } else {
        switch (type) {
        case DATA: {
            // 数据节点
            char* nodedata = ((node_base_data_without_sons*)head)->data;
            auto type_data = what_type(nodedata);
            TVM_space::bytecode_index_t index_argv
                = add(vm, type_data, nodedata);
            if (type_data == string_TICK) {
                add_opcode(new TVM_space::TVM_bytecode {
                    (bytecode_t)byteCodeNumber::LOAD_STRING_, index_argv });
            } else if (type_data == int_TICK || type_data == CONST_TICK) {
                add_opcode(new TVM_space::TVM_bytecode {
                    (bytecode_t)byteCodeNumber::LOAD_INT_, index_argv });
            } else if (type_data == float_TICK) {
                add_opcode(new TVM_space::TVM_bytecode {
                    (bytecode_t)byteCodeNumber::LOAD_FLOAT_, index_argv });
            } else if (type_data == VAR_TICK) {
                add_opcode(new TVM_space::TVM_bytecode {
                    (bytecode_t)byteCodeNumber::LOAD_NAME_, index_argv });
            } else if (type_data == LONG_TICK) {
                add_opcode(new TVM_space::TVM_bytecode {
                    (bytecode_t)byteCodeNumber::LOAD_LONG_, index_argv });
            }
            break;
        }
        case NUMBER: {
            // 整型节点

            break;
        }
        case OPCODE: {
            // 生成字节码, -1代表没有参数
            token_ticks tick = ((node_base_tick_without_sons*)head)->tick;
            add_opcode(build_opcode(tick));
            break;
        }
        default: {
            NOREACH;
        }
        }
    }
}

detail_compiler::detail_compiler(
    compiler_error& error_, TVM_space::TVM_static_data& static_data)
    : error_(error_)
    , static_data(static_data) {
}
}

namespace trc::compiler {
void free_tree(treenode* head) {
    if (head->has_son()) {
        for (const auto i : ((is_not_end_node*)head)->son) {
            free_tree(i);
        }
    }
    delete head;
}

void Compiler(TVM_space::TVM* vm, const std::string& codes) {
    compiler_error error_("__main__");
    // 不会开始解析
    grammar_lex grammar_lexer(codes, &error_);
    /* 正式进入虚拟机字节码生成环节*/
    vm->static_data.ver_ = def::version;
    detail_compiler lex_d(error_, vm->static_data);
    treenode* now_get;
    for (;;) {
        now_get = grammar_lexer.get_node();
        if (now_get == nullptr) {
            break;
        }
        lex_d.compile(vm, now_get);
        free_tree(now_get);
    }
}
}
