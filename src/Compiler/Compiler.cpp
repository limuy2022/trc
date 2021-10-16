/**
 * 字节码最终在此生成，是编译器的另一个核心
 */

#include <string>
#include <vector>
#include <map>
#include "../include/Compiler/compile_share.h"
#include "../include/Compiler/Compiler.h"
#include "../include/code_loader.h"
#include "../include/data.hpp"
#include "../include/type.hpp"
#include "../include/type.h"
#include "../include/Error.h"
#include "../include/memory/mem.h"
#include "../include/node.h"
#include "../include/TVM/TVM_data.h"

using namespace std;

// 解析语法树的索引
static int *lex_num;
// 语法树解析
static vector<treenode *> forest;

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

// 改变变量读写指令，由于有全局和局部的区别，指令的值可能会经常更换
static string LOAD_NAME = LOAD_NAME_, \
 CHANGE_VALUE = CHANGE_VALUE_, \
 STORE_NAME = STORE_NAME_, \
 DEL = DEL_;

static vector<short *> real_compiler(TVM *vm, treenode *head);

static short add(TVM *vm, int data_type, const string &data_value) {
    /**
     * 把数据添加进相对应的vm常量池，并且做一些判断和优化，同时返回数据在常量池中所占的索引
     */

    if (data_type == STRING_TICK) {
        if (!check_in(data_value, vm->static_data.const_s)) {
            const string& fin_str = data_value.substr(1, data_value.length() - 2);
            vm->static_data.const_s.push_back(fin_str);
            return vm->static_data.const_s.size() - 1;
        }
        return index_vector(vm->static_data.const_s, data_value);
    } else if (data_type == INT_TICK) {
        int afdata = to_type<int>(data_value);
        if (!check_in(afdata, vm->static_data.const_i)) {
            vm->static_data.const_i.push_back(afdata);
            return vm->static_data.const_i.size() - 1;
        }
        return index_vector(vm->static_data.const_i, afdata);
    } else if (data_type == FLOAT_TICK) {
        double afdata = to_type<double>(data_value);
        if (!check_in(afdata, vm->static_data.const_f)) {
            vm->static_data.const_f.push_back(afdata);
            return vm->static_data.const_f.size() - 1;
        }
        return index_vector(vm->static_data.const_f, afdata);
    } else if (data_type == VAR_TICK) {
        if (!check_in(data_value, vm->static_data.const_name)) {
            vm->static_data.const_name.push_back(data_value);
            return vm->static_data.const_name.size() - 1;
        }
        return index_vector(vm->static_data.const_name, data_value);
    } else if(data_type == LONG_TICK) {
        if (!check_in(data_value, vm->static_data.const_long)) {
            vm->static_data.const_long.push_back(data_value);
            return vm->static_data.const_long.size() - 1;
        }
        return index_vector(vm->static_data.const_long, data_value);
    } else if (data_type == CONST_TICK) {
        int afdata = change_const[data_value];
        if (!check_in(afdata, vm->static_data.const_i)) {
            vm->static_data.const_i.push_back(afdata);
            return vm->static_data.const_i.size() - 1;
        }
        return index_vector(vm->static_data.const_i, afdata);
    }
}

static short *build_opcode(const string &data, short index = -1) {
    /**
    * 构建字节码，以data在opcodesym_int中的值和index初始化数组
    * data：符号
    * index：索引
    */
    return new short[2]{codes_int[opcodesym_int[data]], index};
}

static short *build_origin(const string &data, short index = -1) {
    /**
    * 建造无需通过符号转化的字节码，助记符
    * data:助记符
    * index：索引
    */
    return new short[2]{codes_int[data], index};
}

static short *build_var(const string &data, short index = -1) {
    /**
     * 构建有关变量的字节码
     * data：符号
     * index：索引
     */

    if (data == "=")
        return new short[2]{codes_int[CHANGE_VALUE], index};
    return new short[2]{codes_int[STORE_NAME], index};
}

static void func_lexer(TVM *vm, treenode *head) {
    /**
     * 函数环境内解析字节码，由于局部函数的存在变得特殊
     * head：当前行 头指针
     */
    STORE_NAME = STORE_LOCAL_;
    LOAD_NAME = LOAD_LOCAL_;
    CHANGE_VALUE = CHANGE_LOCAL_;
    DEL =  DEL_LOCAL_;

    const string &name(head->data);
    int end_line = to_type<int>(head->son[0]->data);

    func_ *fast = new func_(name);
    vm->static_data.funcs[name] = fast;

    // 这几行代码并不保存在主字节码区，而是保存在和函数相关的位置
    for (; *lex_num < end_line; ++(*lex_num))
        fast -> bytecodes.push_back(real_compiler(vm, forest[*lex_num]));
    add(vm, VAR_TICK, name);

    STORE_NAME = STORE_NAME_;
    LOAD_NAME = LOAD_NAME_;
    CHANGE_VALUE = CHANGE_VALUE_;
    DEL = DEL_;
}

vecs2d final_token(const vecs &codes) {
    /**
     * 从string到tokens的转化过程，包括划分，检查，矫正几个部分
     * codes:源代码
     */

    vecs2d token_code;
    token(token_code, codes);
    pre_token_2(token_code);
    check(token_code);
    return token_code;
}

static vector<short *> real_compiler(TVM *vm, treenode *head) {
    /**
     * head:每一行的头指针
     * vm：当时编译的虚拟机
     */
    vector<short *> bytecode_temp;
    size_t n = head->son.size();
    treenode *tree;
    int type;
    string nodedata;
    for (int i = 0; i < n; ++i) {
        tree = (head->son)[i];
        type = tree->type;
        nodedata = tree->data;
        if (type == DATA) {
            // 数据节点
            int type_data = what_type(nodedata);
            short index_argv = add(vm, type_data, nodedata);
            if (type_data == STRING_TICK)
                bytecode_temp.push_back(new short[2]{codes_int[LOAD_STRING_], index_argv});
            else if (type_data == INT_TICK || type_data == CONST_TICK)
                bytecode_temp.push_back(new short[2]{codes_int[LOAD_INT_], index_argv});
            else if (type_data == FLOAT_TICK)
                bytecode_temp.push_back(new short[2]{codes_int[LOAD_FLOAT_], index_argv});
            else if (type_data == VAR_TICK)
                bytecode_temp.push_back(new short[2]{codes_int[LOAD_NAME], index_argv});
            else if(type_data == LONG_TICK)
                bytecode_temp.push_back(new short[2]{codes_int[LOAD_LONG_], index_argv});
        } else if (type == TREE) {
            // 不是数据和传参节点，确认为树
            // 是树则递归，重新生成字节码，进行合并
            auto dg = real_compiler(vm, tree);
            bytecode_temp.insert(bytecode_temp.end(), dg.begin(), dg.end());
        } else if (type == OPCODE) {
            // 生成字节码, -1代表没有参数
            bytecode_temp.push_back(build_opcode(nodedata));
        } else if (type == BUILTIN_FUNC) {
            treenode *code = tree->son[0], *index_ = tree->son[1];
            bytecode_temp.push_back(build_origin(code->data, to_type<short>(index_->data)));
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
            if (!map_check_in_first(vm->static_data.funcs, nodedata))
                send_error(NameError, nodedata.c_str());
            auto *code = tree->son[0], *index_ = tree->son[1];
            bytecode_temp.push_back(build_origin(code->data, to_type<short>(index_->data)));
        }
    }
    return bytecode_temp;
}

void Compiler(TVM *vm, const string &codes) {
    /**
     * 传入一台虚拟机和未编译的代码，将代码编译后填充进vm中
     * vm：可以是编译过的虚拟机
     * codes：源代码
    */

    lex_num = &LINE_NOW;
    // 先释放所有的内存
    free_TVM(vm);

    vecs tmp_;
    pre(tmp_, codes);
    grammar(forest, final_token(tmp_));

    *lex_num = 0;
    /* 正式进入虚拟机字节码生成环节*/
    vm->static_data.ver_ = version;
    for (size_t n = forest.size(); *lex_num < n; ++*lex_num) {
        vm->static_data.byte_codes.push_back(real_compiler(vm, forest[*lex_num]));
        free_tree(forest[*lex_num]);
    }
    *lex_num = 0;
    forest.clear();
}
