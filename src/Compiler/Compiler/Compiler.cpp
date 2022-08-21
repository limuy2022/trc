/**
 * 字节码最终在此生成，是编译器的另一个核心
 */

#include <Compiler/Compiler.h>
#include <Compiler/grammar.h>
#include <Compiler/pri_compiler.hpp>
#include <TVM/TVMdef.h>
#include <TVM/func.h>
#include <base/Error.h>
#include <base/code_loader.h>
#include <base/utils/data.hpp>
#include <language/error.h>
#include <string>
#include <vector>

namespace trc::compiler {
using TVM_space::bytecode_t;

// 将对应字符转化为字节码的过程，此过程会直接通过映射的方式编译出字节码
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
    vm->static_data.byte_codes.emplace_back(opcode, index);
    vm->static_data.line_number_table.push_back(compiler_data.error.line);
    if (compiler_data.error.line != prev_value) {
        line_to_bycodeindex_table.resize(compiler_data.error.line + 1);
        line_to_bycodeindex_table[compiler_data.error.line]
            = vm->static_data.line_number_table.size() - 1;
        prev_value = compiler_data.error.line;
    }
}

void detail_compiler::func_lexer(treenode* head) {
}

TVM_space::bytecode_index_t detail_compiler::add_int(int value) const {
    size_t size = vm->static_data.const_i.size();
    int index = utils::check_in_i(
        value, vm->static_data.const_i.begin(), vm->static_data.const_i.end());
    if (index != -1) {
        return index;
    }
    vm->static_data.const_i.push_back(value);
    return size;
}

TVM_space::bytecode_index_t detail_compiler::add_float(double value) const {
    size_t size = vm->static_data.const_f.size();
    int index = utils::check_in_i(
        value, vm->static_data.const_f.begin(), vm->static_data.const_f.end());
    if (index != -1) {
        return index;
    }
    vm->static_data.const_f.push_back(value);
    return size;
}

TVM_space::bytecode_index_t detail_compiler::add_string(char* value) const {
    int index = utils::str_check_in_i(
        value, vm->static_data.const_s.begin(), vm->static_data.const_s.end());
    size_t size = vm->static_data.const_s.size();
    if (index != -1) {
        free(value);
        return index;
    }
    vm->static_data.const_s.push_back(value);
    return size;
}

TVM_space::bytecode_index_t detail_compiler::add_var_allow_not_in(char* value) {
    return infoenv.get_index_of_globalvar(value, true);
}

TVM_space::bytecode_index_t detail_compiler::add_var_must_in(char* value) {
    return infoenv.get_index_of_globalvar(value, false);
}

TVM_space::bytecode_index_t detail_compiler::add_long(char* value) const {
    size_t size = vm->static_data.const_long.size();
    int index = utils::str_check_in_i(value, vm->static_data.const_long.begin(),
        vm->static_data.const_long.end());
    if (index != -1) {
        free(value);
        return index;
    }
    vm->static_data.const_long.push_back(value);
    return size;
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
    if (head->has_son) {
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
            auto argv_nodes = (is_not_end_node*)*root->son.begin();
            for (auto i : argv_nodes->son) {
                compile(i);
            }
            // 然后编译参数个数
            int num_of_nodes
                = ((is_not_end_node*)*root->son.begin())->son.size();
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
            auto argv_
                = ((node_base_int_without_sons*)*root->son.begin())->value;
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
            compile(*(++root->son.begin()));
            // 处理等式左边的数据
            char* argv_ = ((node_base_data_without_sons*)*root->son.begin())
                              ->swap_string_data();
            auto index_argv = add_var_allow_not_in(argv_);
            add_opcode(build_var(((node_base_tick*)root)->tick), index_argv);
            break;
        }
        case grammar_type::CALL_FUNC: {
            // 调用自定义函数
            // 判断函数是否存在
            char* nodedata = ((node_base_data*)root)->data;
            if (!utils::map_check_in_first(vm->static_data.funcs, nodedata))
                compiler_data.error.send_error_module(
                    error::NameError, language::error::nameerror, nodedata);
            auto* code = (node_base_tick_without_sons*)*root->son.begin();
            auto* index_ = (node_base_int_without_sons*)*root->son.end();
            add_opcode(build_opcode(code->tick), index_->value);
            break;
        }
        default: {
            NOREACH("Unexpeceted grammar treee node type %d.", (int)type);
        }
        }
    } else {
        switch (type) {
        case grammar_type::VAR_NAME: {
            // 变量名节点，生成读取变量的节点
            auto index_argv = add_var_must_in(
                ((node_base_data_without_sons*)head)->swap_string_data());
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
            auto index_argv = add_string(
                ((node_base_data_without_sons*)head)->swap_string_data());
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
            auto index_argv = add_long(
                ((node_base_data_without_sons*)head)->swap_string_data());
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
            NOREACH("Unexpeceted grammar treee node type %d.", (int)type);
        }
        }
    }
}

detail_compiler::detail_compiler(
    compiler_public_data& compiler_data, TVM_space::TVM* vm)
    : infoenv(compiler_data)
    , vm(vm)
    , compiler_data(compiler_data) {
}

void detail_compiler::retie(TVM_space::TVM* vm) {
    this->vm = vm;
}
}

namespace trc::compiler {
void free_tree(treenode* head) {
    if (head->has_son) {
        for (const auto i : ((is_not_end_node*)head)->son) {
            free_tree(i);
        }
    }
    delete head;
}

void detail_compiler::free_detail_compiler() {
    delete &(this->compiler_data.error);
}

/**
 * @brief 负责把节点编译成代码
 * @param grammar_lexer grammar解析器
 * @param lex_dcompiler 负责实现细节的编译器
 * @param vm 虚拟机
 */
static void compile_node(grammar_lex& grammar_lexer,
    detail_compiler& lex_dcompiler, TVM_space::TVM* vm) {
    treenode* now_get;
    for (;;) {
        now_get = grammar_lexer.get_node();
        if (now_get == nullptr) {
            break;
        }
        lex_dcompiler.compile(now_get);
        free_tree(now_get);
    }
    // 设置全局符号表
    vm->static_data.global_symbol_table_size
        = lex_dcompiler.infoenv.get_global_name_size();
}

detail_compiler* Compiler(TVM_space::TVM* vm, const std::string& codes,
    const compiler_option* option, detail_compiler* compiler_ptr,
    bool return_compiler_ptr) {
    /* 正式进入虚拟机字节码生成环节*/

    if (compiler_ptr == nullptr && !return_compiler_ptr) [[likely]] {
        // 如果不需要返回变量信息或者没有提供指定编译器
        // 不需要保存变量信息
        compiler_public_data compiler_data
            = { std::string("__main__"), option };
        // 不会开始解析
        grammar_lex grammar_lexer(codes, compiler_data);
        detail_compiler lex_d(compiler_data, vm);
        compile_node(grammar_lexer, lex_d, vm);
        return nullptr;
    } else if (return_compiler_ptr) {
        // 需要返回保存变量信息(tshell和tdb需要使用)
        auto* compiler_data
            = new compiler_public_data { std::string("__main__"), option };
        grammar_lex grammar_lexer(codes, *compiler_data);
        auto* lex_d = new detail_compiler(*compiler_data, vm);
        compile_node(grammar_lexer, *lex_d, vm);
        return lex_d;
    } else if (compiler_ptr != nullptr) {
        // 使用已经保存了的信息类进行编译
        compiler_ptr->retie(vm);
        grammar_lex grammar_lexer(codes, compiler_ptr->compiler_data);
        compile_node(grammar_lexer, *compiler_ptr, vm);
        return nullptr;
    }
}
}
