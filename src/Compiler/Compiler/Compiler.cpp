/**
 * 字节码最终在此生成，是编译器的另一个核心
 */

#include <Compiler/Compiler.h>
#include <Compiler/grammar.h>
#include <Compiler/pri_compiler.hpp>
#include <TVM/TVM_data.h>
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

void detail_compiler::add_opcode(
    bytecode_t opcode, TVM_space::bytecode_index_t index) {
    vm->static_data.byte_codes.push_back(
        TVM_space::TVM_bytecode { opcode, index });
    vm->static_data.line_number_table.push_back(error_.line);
    if (error_.line != prev_value) {
        line_to_bycodeindex_table.resize(error_.line + 1);
        line_to_bycodeindex_table[error_.line]
            = vm->static_data.line_number_table.size() - 1;
        prev_value = error_.line;
    }
}

void detail_compiler::func_lexer(treenode* head) {
}

TVM_space::bytecode_index_t detail_compiler::add_int(int value) {
    size_t size = vm->static_data.const_i.size();
    int index = utils::check_in_i(value, vm->static_data.const_i.begin() + 1,
        vm->static_data.const_i.end());
    if (index != -1) {
        return index + 1;
    } else {
        vm->static_data.const_i.push_back(value);
        return size;
    }
}

TVM_space::bytecode_index_t detail_compiler::add_float(float value) {
    size_t size = vm->static_data.const_f.size();
    int index = utils::check_in_i(value, vm->static_data.const_f.begin() + 1,
        vm->static_data.const_f.end());
    if (index != -1) {
        return index + 1;
    } else {
        vm->static_data.const_f.push_back(value);
        return size;
    }
}

TVM_space::bytecode_index_t detail_compiler::add_string(const char* value) {
    int index = utils::str_check_in_i(value,
        vm->static_data.const_s.begin() + 1, vm->static_data.const_s.end());
    size_t size = vm->static_data.const_s.size();
    if (index != -1) {
        return index + 1;
    } else {
        char* copy = new char[strlen(value) + 1];
        strcpy(copy, value);
        vm->static_data.const_s.push_back(copy);
        return size;
    }
}

TVM_space::bytecode_index_t detail_compiler::add_var(const char* value) {
    return infoenv.get_index_of_globalvar(value);
}

TVM_space::bytecode_index_t detail_compiler::add_long(const char* value) {
    size_t size = vm->static_data.const_long.size();
    int index
        = utils::str_check_in_i(value, vm->static_data.const_long.begin() + 1,
            vm->static_data.const_long.end());
    if (index != -1) {
        return index + 1;
    } else {
        char* copy = new char[strlen(value) + 1];
        strcpy(copy, value);
        vm->static_data.const_long.push_back(copy);
        return size;
    }
}

bytecode_t detail_compiler::build_opcode(token_ticks symbol) {
    return (bytecode_t)opcodesym_int[(int)symbol];
}

bytecode_t detail_compiler::build_var(token_ticks data) {
    if (data == token_ticks::ASSIGN)
        return (bytecode_t)byteCodeNumber::CHANGE_VALUE_;
    return (bytecode_t)byteCodeNumber::STORE_NAME_;
}

void detail_compiler::compile(treenode* head) {
    grammar_type type = head->type;
    if (head->has_son()) {
        auto* root = (is_not_end_node*)head;
        switch (type) {
        case grammar_type::EXPR:
        // 运算符表达式或条件表达式
        case grammar_type::TREE: {
            // 不是数据和传参节点，确认为树
            for (auto i : root->son) {
                compile(i);
            }
            break;
        }
        case grammar_type::BUILTIN_FUNC: {
            // 内置函数
            // 先递归编译参数
            auto argv_nodes = (is_not_end_node*)root->son[0];
            for (size_t i = 0, n = argv_nodes->son.size(); i < n; ++i) {
                compile(argv_nodes->son[i]);
            }
            // 然后编译参数个数
            int num_of_nodes = ((is_not_end_node*)(root->son[0]))->son.size();
            add_opcode(
                (bytecode_t)byteCodeNumber::LOAD_INT_, add_int(num_of_nodes));
            // 最后加上调用函数
            // 内置函数以编码形式保存，这是它的位置
            int function_index = ((node_base_int*)root)->value;
            add_opcode(
                (bytecode_t)byteCodeNumber::CALL_BUILTIN_, function_index);
            break;
        }
        case grammar_type::OPCODE_ARGV: {
            // 带参数字节码
            // 参数
            auto argv_ = ((node_base_int_without_sons*)(root->son[0]))->value;
            add_opcode(
                build_opcode(((node_base_tick*)root)->tick), add_int(argv_));
            break;
        }
        case grammar_type::FUNC_DEFINE: {
            // 函数定义
            func_lexer(head);
            break;
        }
        case grammar_type::VAR_DEFINE: {
            // 变量定义
            // 处理等式右边的数据
            compile(root->son[1]);
            // 处理等式左边的数据
            auto* argv_ = (node_base_string_without_sons*)root->son[0];
            short index_argv = add_var(argv_->data);
            add_opcode(build_var(((node_base_tick*)root)->tick), index_argv);
            break;
        }
        case grammar_type::CALL_FUNC: {
            // 调用自定义函数
            // 判断函数是否存在
            char* nodedata = ((node_base_data*)root)->data;
            if (!utils::map_check_in_first(vm->static_data.funcs, nodedata))
                error_.send_error_module(
                    error::NameError, language::error::nameerror, nodedata);
            auto* code = (node_base_tick_without_sons*)root->son[0];
            auto* index_ = (node_base_int_without_sons*)root->son[1];
            add_opcode(build_opcode(code->tick), index_->value);
            break;
        }
        default: {
            NOREACH;
        }
        }
    } else {
        switch (type) {
        case grammar_type::VAR_NAME: {
            // 变量名节点，生成读取变量的节点
            char* nodedata = ((node_base_data_without_sons*)head)->data;
            auto index_argv = add_var(nodedata);
            add_opcode((bytecode_t)byteCodeNumber::LOAD_NAME_, index_argv);
            break;
        }
        case grammar_type::NUMBER: {
            // 整型节点
            auto value = ((node_base_int_without_sons*)head)->value;
            auto index_argv = add_int(value);
            add_opcode((bytecode_t)byteCodeNumber::LOAD_INT_, index_argv);
            break;
        }
        case grammar_type::STRING: {
            // 字符串节点
            auto value = ((node_base_string_without_sons*)head)->data;
            auto index_argv = add_string(value);
            add_opcode((bytecode_t)byteCodeNumber::LOAD_STRING_, index_argv);
            break;
        }
        case grammar_type::FLOAT: {
            // 浮点型节点
            auto value = ((node_base_float_without_sons*)head)->value;
            auto index_argv = add_float(value);
            add_opcode((bytecode_t)byteCodeNumber::LOAD_FLOAT_, index_argv);
            break;
        }
        case grammar_type::LONG_INT: {
            // 长整型节点
            auto value = ((node_base_data_without_sons*)head)->data;
            auto index_argv = add_long(value);
            add_opcode((bytecode_t)byteCodeNumber::LOAD_LONG_, index_argv);
            break;
        }
        case grammar_type::OPCODE: {
            // 生成字节码, 0代表没有参数
            token_ticks tick = ((node_base_tick_without_sons*)head)->tick;
            add_opcode(build_opcode(tick), 0);
            break;
        }
        default: {
            NOREACH;
        }
        }
    }
}

detail_compiler::detail_compiler(compiler_error& error_, TVM_space::TVM* vm)
    : error_(error_)
    , vm(vm) {
}

void detail_compiler::retie(TVM_space::TVM* vm) {
    this->vm = vm;
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

void detail_compiler::free_detail_compiler() {
    delete &this->error_;
}

detail_compiler* Compiler(TVM_space::TVM* vm, const std::string& codes,
    detail_compiler* compiler_ptr, bool return_compiler_ptr) {
    /* 正式进入虚拟机字节码生成环节*/
    vm->static_data.ver_ = def::version;
    treenode* now_get;
    if (compiler_ptr == nullptr && return_compiler_ptr == false) [[likely]] {
        // 如果不需要返回变量信息或者没有提供指定编译器
        // 不需要保存变量信息
        compiler_error error_("__main__");
        // 不会开始解析
        grammar_lex grammar_lexer(codes, &error_);
        detail_compiler lex_d(error_, vm);

        for (;;) {
            now_get = grammar_lexer.get_node();
            if (now_get == nullptr) {
                break;
            }
            lex_d.compile(now_get);
            free_tree(now_get);
        }
        // 设置全局符号表
        vm->static_data.global_symbol_table_size
            = lex_d.infoenv.get_global_name_size();
        return nullptr;
    } else if (return_compiler_ptr == true) {
        // 需要返回保存变量信息(tshell和tdb需要使用)
        compiler_error* error_ = new compiler_error("__main__");
        grammar_lex grammar_lexer(codes, error_);
        detail_compiler* lex_d = new detail_compiler(*error_, vm);

        for (;;) {
            now_get = grammar_lexer.get_node();
            if (now_get == nullptr) {
                break;
            }
            lex_d->compile(now_get);
            free_tree(now_get);
        }
        // 设置全局符号表
        vm->static_data.global_symbol_table_size
            = lex_d->infoenv.get_global_name_size();
        return lex_d;
    } else if (compiler_ptr != nullptr) {
        // 使用已经保存了的信息类进行编译
        compiler_ptr->retie(vm);
        grammar_lex grammar_lexer(codes, &compiler_ptr->error_);
        for (;;) {
            now_get = grammar_lexer.get_node();
            if (now_get == nullptr) {
                break;
            }
            compiler_ptr->compile(now_get);
            free_tree(now_get);
        }
    }
    // 设置全局符号表
    vm->static_data.global_symbol_table_size
        = compiler_ptr->infoenv.get_global_name_size();
    return nullptr;
}
}
