/**
 * 字节码生成器
 */

#include <Compiler/Compiler.h>
#include <Compiler/grammar.h>
#include <Compiler/pri_compiler.hpp>
#include <TVM/TVMdef.h>
#include <base/code_loader.h>
#include <base/utils/data.hpp>
#include <string>
#include <vector>

namespace trc::compiler {
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
    byteCodeNumber::ASSERT_,
};

void detail_compiler::generate_line_table() {
    vm->line_number_table.push_back(compiler_data.error.line);
    if (compiler_data.error.line != prev_value) {
        line_to_bycodeindex_table.resize(compiler_data.error.line + 1);
        line_to_bycodeindex_table[compiler_data.error.line]
            = vm->line_number_table.size() - 1;
        prev_value = compiler_data.error.line;
    }
}

void detail_compiler::add_opcode(byteCodeNumber opcode,
    TVM_space::bytecode_index_t index, TVM_space::struct_codes& bytecode) {
    bytecode.emplace_back((bytecode_t)opcode, index);
    generate_line_table();
}

TVM_space::bytecode_index_t detail_compiler::add_int(int value) const {
    size_t size = vm->const_i.size;
    int index = utils::check_in_i(value, vm->const_i.array, vm->const_i.end());
    if (index != -1) {
        return index;
    }
    vm->const_i.array[vm->const_i.size++] = value;
    return size;
}

TVM_space::bytecode_index_t detail_compiler::add_float(double value) const {
    size_t size = vm->const_f.size;
    int index = utils::check_in_i(value, vm->const_f.array, vm->const_f.end());
    if (index != -1) {
        return index;
    }
    vm->const_f.array[vm->const_f.size++] = value;
    return size;
}

TVM_space::bytecode_index_t detail_compiler::add_string(char* value) const {
    int index
        = utils::str_check_in_i(value, vm->const_s.array, vm->const_s.end());
    size_t size = vm->const_s.size;
    if (index != -1) {
        free(value);
        return index;
    }
    vm->const_s.array[vm->const_s.size++] = value;
    return size;
}

TVM_space::bytecode_index_t detail_compiler::add_long(char* value) const {
    size_t size = vm->const_long.size;
    int index = utils::str_check_in_i(
        value, vm->const_long.array, vm->const_long.end());
    if (index != -1) {
        free(value);
        return index;
    }
    vm->const_long.array[vm->const_long.size++] = value;
    return size;
}

byteCodeNumber detail_compiler::build_opcode(token_ticks symbol) {
    return opcodesym_int[(int)symbol];
}

void detail_compiler::add_var(byteCodeNumber bycode, is_not_end_node* root,
    basic_compile_env& localinfo, size_t index) {
    // 处理等式右边的数据
    compile(*(++root->son.begin()), localinfo);
    add_opcode(bycode, index, localinfo.bytecode);
}

void detail_compiler::compile(treenode* head, basic_compile_env& localinfo) {
    grammar_type type = head->type;
    if (head->has_son) {
        auto* root = (is_not_end_node*)head;
        switch (type) {
            // 运算符表达式或条件表达式
        case grammar_type::EXPR:
        // 树
        case grammar_type::TREE: {
            for (auto i : root->son) {
                compile(i, localinfo);
            }
            break;
        }
        case grammar_type::BUILTIN_FUNC: {
            // 内置函数
            // 先递归编译参数
            for (auto i : root->son) {
                compile(i, localinfo);
            }
            // 然后编译参数个数
            int num_of_nodes = root->son.size();
            add_opcode(byteCodeNumber::LOAD_INT_, add_int(num_of_nodes),
                localinfo.bytecode);
            // 最后加上调用函数
            // 内置函数以编码形式保存，这是它的位置
            int function_index = ((node_base_int*)root)->value;
            add_opcode(byteCodeNumber::CALL_BUILTIN_, function_index,
                localinfo.bytecode);
            break;
        }
        case grammar_type::OPCODE_ARGV: {
            // 带参数字节码
            // 参数
            auto argv_
                = ((node_base_int_without_sons*)root->son.front())->value;
            add_opcode(build_opcode(((node_base_tick*)root)->tick),
                add_int(argv_), localinfo.bytecode);
            break;
        }
        case grammar_type::FUNC_DEFINE: {
            // 首先添加函数定义
            auto funcname = ((node_base_data*)root)->swap_string_data();
            infoenv.add_function(funcname);
            size_t func_index = infoenv.get_func_size();
            // 然后为函数新建一个新的局部环境，进行编译
            basic_compile_env localfuncenv(
                compiler_data, vm->funcs[func_index].bytecodes);
            // 编译构建变量的代码
            auto index = root->son.begin();
            for (auto i : ((is_not_end_node*)(*index))->son) {
                add_opcode(byteCodeNumber::STORE_LOCAL_,
                    localfuncenv.add_var(((node_base_string_without_sons*)i)
                                             ->swap_string_data()),
                    localinfo.bytecode);
            }
            // 编译函数体
            // 指向第一条函数体代码
            index++;
            for (auto end = root->son.end(); index != end; ++index) {
                compile(*index, localfuncenv);
            }
            // 设置函数符号表
            vm->funcs[func_index].symbol_form_size
                = localfuncenv.get_name_size();
            break;
        }
        case grammar_type::VAR_DEFINE: {
            // 变量定义
            // 处理等式左边的数据
            char* argv_ = ((node_base_string_without_sons*)root->son.front())
                              ->swap_string_data();
            auto index_argv = localinfo.add_var(argv_);
            if (&localinfo == &infoenv) {
                // 全局环境
                add_var(
                    byteCodeNumber::STORE_NAME_, root, localinfo, index_argv);
            } else {
                // 局部环境
                add_var(
                    byteCodeNumber::STORE_LOCAL_, root, localinfo, index_argv);
            }
            break;
        }
        case grammar_type::VAR_ASSIGN: {
            // 变量赋值
            char* argv_ = ((node_base_string_without_sons*)root->son.front())
                              ->swap_string_data();
            auto index_argv = localinfo.get_index_of_var(argv_, true);
            if (&localinfo == &infoenv) {
                // 全局环境
                add_var(
                    byteCodeNumber::CHANGE_VALUE_, root, localinfo, index_argv);
            } else {
                // 局部环境
                add_var(
                    byteCodeNumber::CHANGE_LOCAL_, root, localinfo, index_argv);
            }
            break;
        }
        case grammar_type::CALL_FUNC: {
            // 调用自定义函数
            char* funcname = ((node_base_data*)root)->value;
            size_t index = infoenv.get_index_of_function(funcname);
            // 编译参数
            for (auto i : root->son) {
                compile(i, localinfo);
            }
            add_opcode(
                byteCodeNumber::CALL_FUNCTION_, index, localinfo.bytecode);
            break;
        }
            // 需要跳转的语句块
        case grammar_type::IF_BLOCK: {
            add_block<false>(root, localinfo);
            break;
        }
        case grammar_type::WHILE_BLOCK: {
            add_block<true>(root, localinfo);
            break;
        }
        default: {
            NOREACH("Unexpeceted grammar tree node type %d", (int)type);
        }
        }
    } else {
        switch (type) {
        case grammar_type::VAR_NAME: {
            // 变量名节点，生成读取变量的字节码
            auto varname = ((node_base_string_without_sons*)head)->value;
            // 获取变量的索引
            size_t index_argv;
            // 首先尝试在局部变量中查找
            if (&localinfo == &infoenv) {
                // 相等，说明现在处于全局作用域中
                index_argv = localinfo.get_index_of_var(varname, true);
                add_opcode(
                    byteCodeNumber::LOAD_NAME_, index_argv, localinfo.bytecode);
            } else {
                index_argv = localinfo.get_index_of_var(varname, false);
                if (index_argv == size_tmax) {
                    // 如果在局部变量中没有查找到
                    index_argv = infoenv.get_index_of_var(varname, true);
                    add_opcode(byteCodeNumber::LOAD_NAME_, index_argv,
                        localinfo.bytecode);
                } else {
                    add_opcode(byteCodeNumber::LOAD_LOCAL_, index_argv,
                        localinfo.bytecode);
                }
            }
            break;
        }
        case grammar_type::NUMBER: {
            // 整型节点
            auto value = ((node_base_int_without_sons*)head)->value;
            auto index_argv = add_int(value);
            add_opcode(
                byteCodeNumber::LOAD_INT_, index_argv, localinfo.bytecode);
            break;
        }
        case grammar_type::STRING: {
            // 字符串节点
            auto index_argv = add_string(
                ((node_base_string_without_sons*)head)->swap_string_data());
            add_opcode(
                byteCodeNumber::LOAD_STRING_, index_argv, localinfo.bytecode);
            break;
        }
        case grammar_type::FLOAT: {
            // 浮点型节点
            auto value = ((node_base_float_without_sons*)head)->value;
            auto index_argv = add_float(value);
            add_opcode(
                byteCodeNumber::LOAD_FLOAT_, index_argv, localinfo.bytecode);
            break;
        }
        case grammar_type::LONG_INT: {
            // 长整型节点
            auto index_argv = add_long(
                ((node_base_string_without_sons*)head)->swap_string_data());
            add_opcode(
                byteCodeNumber::LOAD_LONG_, index_argv, localinfo.bytecode);
            break;
        }
        case grammar_type::OPCODE: {
            // 生成字节码, 0代表没有参数
            token_ticks tick = ((node_base_tick_without_sons*)head)->tick;
            add_opcode(build_opcode(tick), 0, localinfo.bytecode);
            break;
        }
        default: {
            NOREACH("Unexpeceted grammar tree node type %d.", (int)type);
        }
        }
    }
}

detail_compiler::detail_compiler(
    compiler_public_data& compiler_data, TVM_space::TVM_static_data* vm)
    : infoenv(compiler_data, vm->byte_codes)
    , vm(vm)
    , compiler_data(compiler_data) {
}

void detail_compiler::retie(TVM_space::TVM_static_data* vm) {
    this->vm = vm;
}

void detail_compiler::compile_node(grammar_lex& grammar_lexer) {
    treenode* root = grammar_lexer.compile_all();
    // 申请虚拟机常量池的内存，统计元素个数时没有去重，可能会多申请，后面会压缩掉多余的储存空间
    vm->const_i.array = new int[compiler_data.int_size];
    vm->const_f.array = new double[compiler_data.float_size];
    vm->const_s.array = new char*[compiler_data.string_size];
    vm->const_long.array = new char*[compiler_data.long_int_size];
    vm->funcs.array = new TVM_space::func_[compiler_data.func_size];
    // 对于全局对象，局部作用域等于全局作用域
    compile(root, infoenv);
    delete root;
    // 设置全局符号表
    vm->global_symbol_table_size = infoenv.get_name_size();
    // 压缩内存
    vm->compress_memory();
}

detail_compiler* Compiler(TVM_space::TVM_static_data& vm,
    const std::string& codes, const compiler_option& option,
    detail_compiler* compiler_ptr, bool return_compiler_ptr) {
    if (compiler_ptr == nullptr && !return_compiler_ptr) [[likely]] {
        // 如果不需要返回变量信息或者没有提供指定编译器
        // 不需要保存变量信息
        compiler_public_data compiler_data
            = { std::string("__main__"), option };
        grammar_lex grammar_lexer(codes, compiler_data);
        detail_compiler lex_d(compiler_data, &vm);
        lex_d.compile_node(grammar_lexer);
        return nullptr;
    } else if (return_compiler_ptr) {
        // 需要返回保存变量信息(tshell和tdb需要使用)
        auto* compiler_data
            = new compiler_public_data { std::string("__main__"), option };
        grammar_lex grammar_lexer(codes, *compiler_data);
        auto* lex_d = new detail_compiler(*compiler_data, &vm);
        lex_d->compile_node(grammar_lexer);
        return lex_d;
    } else if (compiler_ptr != nullptr) {
        // 使用已经保存了的信息类进行编译
        compiler_ptr->retie(&vm);
        grammar_lex grammar_lexer(codes, compiler_ptr->compiler_data);
        compiler_ptr->compile_node(grammar_lexer);
        return nullptr;
    }
    NOREACH("The argvs of compiler are wrong");
}
}
