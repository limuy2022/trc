/**
 * 字节码最终在此生成，是编译器的另一个核心
 */

#include <string>
#include <vector>
#include <map>
#include "Compiler/compile_def.hpp"
#include "Compiler/Compiler.h"
#include "base/code_loader.h"
#include "base/utils/data.hpp"
#include "base/utils/type.hpp"
#include "base/Error.h"
#include "TVM/func.h"
#include "base/memory/memory.h"
#include "TVMbase/memory.h"
#include "compile_env.h"

using namespace std;

// 改变变量读写指令，由于有全局和局部的区别，指令的值可能会经常更换
static string LOAD_NAME = LOAD_NAME_, \
 CHANGE_VALUE = CHANGE_VALUE_, \
 STORE_NAME = STORE_NAME_, \
 DEL = DEL_;

// 将对应字符转化为字节码的过程，此过程仅能编译出助记符，并不能完全编译出字节码
static map<string, string> opcodesym_int = {
        {"del",    DEL_},
        {"import", IMPORT_},
        {"+",      ADD_},
        {"-",      SUB_},
        {"*",      MUL_},
        {"/",      DIV_},
        {"**",     POW_},
        {"%",      MOD_},
        {"//",     ZDIV_},
        {"==",     EQUAL_},
        {"!=",     UNEQUAL_},
        {">=",     GREATER_EQUAL_},
        {"<=",     LESS_EQUAL_},
        {"<",      LESS_},
        {">",      GREATER_},
        {"assert", ASSERT_},
        {"while",  IF_FALSE_GOTO_},
        {"if",     IF_FALSE_GOTO_},
        {"goto",   GOTO_},
        {"not",    NOT_},
        {"and",    AND_},
        {"or",     OR_},
};

namespace trc {
    namespace compiler {
        class detail_compiler {
        public:
            detail_compiler(int *, vector<treenode*>*);

            vector<TVM_space::TVM_bytecode*> real_compiler(TVM_space::TVM *vm, treenode *head);
        private:
            short add(TVM_space::TVM *vm, int data_type, char *data_value);

            TVM_space::TVM_bytecode *build_opcode(const string &data, short index = -1);

            TVM_space::TVM_bytecode *build_origin(const string &data, short index = -1);

            TVM_space::TVM_bytecode *build_var(const string &data, short index = -1);

            void func_lexer(TVM_space::TVM *vm, treenode *head);

            int *line;

            vector<treenode*>* gline;
        };

        void detail_compiler::func_lexer(TVM_space::TVM *vm, treenode *head) {
            /**
             * 函数环境内解析字节码，由于局部函数的存在变得特殊
             * head：当前行 头指针
             */
            STORE_NAME = STORE_LOCAL_;
            LOAD_NAME = LOAD_LOCAL_;
            CHANGE_VALUE = CHANGE_LOCAL_;
            DEL = DEL_LOCAL_;

            char *name = head->data;
            int end_line = atoi(head->son[0]->data);

            auto *fast = new TVM_space::func_{name};
            vm->static_data.funcs[name] = fast;

            // 这几行代码并不保存在主字节码区，而是保存在和函数相关的位置
            int l_t = *line;
            for (; l_t < end_line; ++l_t)
                fast->bytecodes.push_back(real_compiler(vm, gline->operator[](l_t)));
            add(vm, VAR_TICK, name);

            STORE_NAME = STORE_NAME_;
            LOAD_NAME = LOAD_NAME_;
            CHANGE_VALUE = CHANGE_VALUE_;
            DEL = DEL_;
        }

        short detail_compiler::add(TVM_space::TVM *vm, int data_type, char *data_value) {
            /**
             * 把数据添加进相对应的vm常量池，并且做一些判断和优化，同时返回数据在常量池中所占的索引
             */

            if (data_type == string_TICK) {
                size_t index_tmp = vm->static_data.const_s.size();
                if (!utils::check_in(data_value, vm->static_data.const_s)) {
                    vm->static_data.const_s.push_back(data_value);
                    return index_tmp;
                }
                return utils::index_vector(vm->static_data.const_s, data_value);
            } else if (data_type == int_TICK) {
                int afdata = atoi(data_value);
                size_t index_tmp = vm->static_data.const_i.size();
                if (!utils::check_in(afdata, vm->static_data.const_i)) {
                    vm->static_data.const_i.push_back(afdata);
                    return index_tmp;
                }
                return utils::index_vector(vm->static_data.const_i, afdata);
            } else if (data_type == float_TICK) {
                double afdata = atof(data_value);
                size_t index_tmp = vm->static_data.const_f.size();
                if (!utils::check_in(afdata, vm->static_data.const_f)) {
                    vm->static_data.const_f.push_back(afdata);
                    return index_tmp;
                }
                return utils::index_vector(vm->static_data.const_f, afdata);
            } else if (data_type == VAR_TICK) {
                size_t index_tmp = vm->static_data.const_name.size();
                if (!utils::check_in(data_value, vm->static_data.const_name)) {
                    vm->static_data.const_name.push_back(data_value);
                    return index_tmp;
                }
                return utils::index_vector(vm->static_data.const_name, data_value);
            } else if (data_type == LONG_TICK) {
                size_t index_tmp = vm->static_data.const_long.size();
                if (!utils::check_in(data_value, vm->static_data.const_long)) {
                    vm->static_data.const_long.push_back(data_value);
                    return index_tmp;
                }
                return utils::index_vector(vm->static_data.const_long, data_value);
            } else if (data_type == CONST_TICK) {
                int afdata = change_const[data_value];
                size_t index_tmp = vm->static_data.const_i.size();
                if (!utils::check_in(afdata, vm->static_data.const_i)) {
                    vm->static_data.const_i.push_back(afdata);
                    return index_tmp;
                }
                return utils::index_vector(vm->static_data.const_i, afdata);
            }
        }

        TVM_space::TVM_bytecode *detail_compiler::build_opcode(const string &data, short index) {
            /**
            * 构建字节码，以data在opcodesym_int中的值和index初始化数组
            * data：符号
            * index：索引
            */
            return new TVM_space::TVM_bytecode{loader::codes_int[opcodesym_int[data]], index};
        }

        TVM_space::TVM_bytecode *detail_compiler::build_origin(const string &data, short index) {
            /**
            * 建造无需通过符号转化的字节码，助记符
            * data:助记符
            * index：索引
            */
            return new TVM_space::TVM_bytecode{loader::codes_int[data], index};
        }

        TVM_space::TVM_bytecode *detail_compiler::build_var(const string &data, short index) {
            /**
             * 构建有关变量的字节码
             * data：符号
             * index：索引
             */

            if (data == "=")
                return new TVM_space::TVM_bytecode{loader::codes_int[CHANGE_VALUE], index};
            return new TVM_space::TVM_bytecode{loader::codes_int[STORE_NAME], index};
        }

        vector<TVM_space::TVM_bytecode*> detail_compiler::real_compiler(TVM_space::TVM *vm, treenode *head) {
            /**
             * head:每一行的头指针
             * vm：当时编译的虚拟机
             */
            vector<TVM_space::TVM_bytecode*> bytecode_temp;
            size_t n = head->son.size();
            treenode *tree;
            int type;
            char* nodedata;
            for (int i = 0; i < n; ++i) {
                tree = (head->son)[i];
                type = tree->type;
                nodedata = tree->data;
                if (type == DATA) {
                    // 数据节点
                    int type_data = what_type(nodedata);
                    short index_argv = add(vm, type_data, nodedata);
                    if (type_data == string_TICK)
                        bytecode_temp.push_back(new TVM_space::TVM_bytecode{loader::codes_int[LOAD_STRING_], index_argv});
                    else if (type_data == int_TICK || type_data == CONST_TICK)
                        bytecode_temp.push_back(new TVM_space::TVM_bytecode{loader::codes_int[LOAD_INT_], index_argv});
                    else if (type_data == float_TICK)
                        bytecode_temp.push_back(new TVM_space::TVM_bytecode{loader::codes_int[LOAD_FLOAT_], index_argv});
                    else if (type_data == VAR_TICK)
                        bytecode_temp.push_back(new TVM_space::TVM_bytecode{loader::codes_int[LOAD_NAME_], index_argv});
                    else if (type_data == LONG_TICK)
                        bytecode_temp.push_back(new TVM_space::TVM_bytecode{loader::codes_int[LOAD_LONG_], index_argv});
                } else if (type == TREE) {
                    // 不是数据和传参节点，确认为树
                    // 是树则递归，重新生成字节码，进行合并
                    const auto& dg = real_compiler(vm, tree);
                    bytecode_temp.insert(bytecode_temp.end(), dg.begin(), dg.end());
                } else if (type == OPCODE) {
                    // 生成字节码, -1代表没有参数
                    bytecode_temp.push_back(build_opcode(nodedata));
                } else if (type == BUILTIN_FUNC) {
                    treenode *code = tree->son[0], *index_ = tree->son[1];
                    bytecode_temp.push_back(build_origin(code->data, utils::to_type<short>(index_->data)));
                } else if (type == ORIGIN) {
                    // 无法通过符号转化的助记符
                    bytecode_temp.push_back(build_origin(nodedata));
                } else if (type == OPCODE_ARGV) {
                    // 带参数字节码
                    // 参数
                    treenode *argv_ = tree->son[0];
                    int type_data = what_type(argv_->data);
                    bytecode_temp.push_back(build_opcode(nodedata, add(vm, type_data, argv_->data)));
                } else if (type == FUNC_DEFINE) {
                    // 函数定义
                    func_lexer(vm, head);
                } else if (type == VAR_DEFINE) {
                    treenode *argv_ = tree->son[0];
                    int type_data = what_type(argv_->data);
                    short index_argv = add(vm, type_data, argv_->data);
                    bytecode_temp.push_back(build_var(nodedata, index_argv));
                } else if (type == CALL_FUNC) {
                    // 调用自定义函数
                    // 判断函数是否存在
                    if (!utils::map_check_in_first(vm->static_data.funcs, nodedata))
                        error::send_error(error::NameError, nodedata);
                    auto *code = tree->son[0], *index_ = tree->son[1];
                    bytecode_temp.push_back(build_origin(code->data, utils::to_type<short>(index_->data)));
                }
            }
            return bytecode_temp;
        }

        detail_compiler::detail_compiler(int *line, vector<treenode*>* gline) : 
            line(line),
            gline(gline) {}
    }
}

namespace trc {
    namespace compiler {
        void free_tree(const compiler::treenode *head) {
            /**
            * 释放整棵树的内存
            * 递归释放内存
            * delete
            */
            for (const auto &i: head->son)
                if (!i->son.empty())
                    free_tree(i);
                else
                    delete i;
            delete head;
            head = nullptr;
        }

        compiler_t Compiler;

        void compiler_t::operator()(TVM_space::TVM *vm, const string &codes) {
            /**
             * 传入一台虚拟机和未编译的代码，将代码编译后填充进vm中
             * vm：可以是编译过的虚拟机
             * codes：源代码
            */
            lex_num = &LINE_NOW;
            // 先释放所有的内存
            TVM_space::free_TVM(vm);
            // 不会开始解析
            grammar_ = new grammar_lex(codes);
            /* 正式进入虚拟机字节码生成环节*/
            vm->static_data.ver_ = def::version;
            vector<treenode*>* gline = grammar_->compile_nodes();
            detail_compiler lex_d(this->lex_num, gline);
            for(auto i: *gline) {
                vm->static_data.byte_codes.push_back(lex_d.real_compiler(vm, i));
                free_tree(i);
                ++(*(this->lex_num));
            }
            lex_num = 0;
            delete grammar_;
        }
    }
}
