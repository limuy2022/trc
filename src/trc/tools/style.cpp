#include <Compiler/pri_compiler.hpp>
#include <Compiler/token.h>
#include <base/func_loader.h>
#include <base/utils/filesys.h>
#include <cstdio>
#include <string>
#include <tools.h>

namespace trc::tools {
static compiler::compiler_public_data shared_data {
    compiler::compiler_error("__main__"), &compiler::nooptimize_option
};

namespace tools_in {
    /**
     * 编译if和while等块状代码
     * @param file 文件指针
     * @param node 当前语法树的节点
     * @param tabsize 缩进
     * @param name 语句块的名字
     */
    static void block_format(
        FILE* file, is_not_end_node* node, int tabsize, const char* name) {
        fprintf(file, "%s", name);
        // 开头表达式
        auto iter = node->son.begin();
        __style_file(file, *iter, 0);
        iter++;
        // 然后编译其它所有的节点
        for (; iter != node->son.end(); ++iter) {
            __style_file(file, *iter, tabsize + 1);
        }
    }

    void __style_file(FILE* file, treenode* node, int tabsize) {
        // 一棵语法树就是一段代码
        for (int i = 1; i <= tabsize; ++i) {
            fputc('\t', file);
        }
        grammar_type type = node->type;
        if (node->has_son) {
            auto* root = (is_not_end_node*)node;
            switch (type) {
                // 运算符表达式或条件表达式
            case grammar_type::EXPR: {
                for (auto i : root->son) {
                    __style_file(file, i, tabsize);
                }
                break;
            }
            // 树
            case grammar_type::TREE: {
                for (auto i : root->son) {
                    __style_file(file, i, tabsize + 1);
                }
                break;
            }
            case grammar_type::BUILTIN_FUNC: {
                // 内置函数
                // 先递归编译参数
                int function_index = ((node_base_int*)root)->value;
                fprintf(file, "%s(", loader::num_func[function_index]);
                auto i = root->son.begin();
                auto end = root->son.end();
                end--;
                for (; i != end; ++i) {
                    __style_file(file, *i, 0);
                    fprintf(file, ", ");
                }
                __style_file(file, *i, 0);
                fputc(')', file);
                break;
            }
            case grammar_type::FUNC_DEFINE: {
                // 首先添加函数定义
                auto funcname = ((node_base_data*)root)->value;
                fprintf(file, "func %s(", funcname);
                // 编译构建变量的代码
                auto index = root->son.begin();
                auto i = ((is_not_end_node*)(*index))->son.begin(),
                     end_iter = ((is_not_end_node*)(*index))->son.end();
                end_iter--;
                for (; i != end_iter; ++i) {
                    fprintf(file, "%s, ",
                        ((node_base_string_without_sons*)*i)->value);
                }
                fprintf(
                    file, "%s) {", ((node_base_string_without_sons*)*i)->value);
                // 编译函数体
                // 指向第一条函数体代码
                index++;
                for (auto end = root->son.end(); index != end; ++index) {
                    __style_file(file, *index, tabsize + 1);
                }
                break;
            }
            case grammar_type::VAR_DEFINE: {
                // 变量定义
                auto iter = root->son.begin();
                // 处理等式左边的数据
                char* argv_ = ((node_base_string_without_sons*)*iter)->value;
                fprintf(file, "%s := ", argv_);
                // 处理等式右边的数据
                iter++;
                __style_file(file, *iter, 0);
                break;
            }
            case grammar_type::VAR_ASSIGN: {
                // 变量赋值
                auto iter = root->son.begin();
                // 处理等式左边的数据
                char* argv_ = ((node_base_string_without_sons*)*iter)
                                  ->swap_string_data();
                fprintf(file, "%s = ", argv_);
                // 处理等式右边的数据
                iter++;
                __style_file(file, *iter, 0);
                break;
            }
            case grammar_type::CALL_FUNC: {
                // 调用自定义函数
                char* funcname = ((node_base_data*)root)->value;
                fprintf(file, "%s", funcname);
                // 编译参数
                for (auto i : root->son) {
                    __style_file(file, i, 0);
                }
                break;
            }
                // 需要跳转的语句块
            case grammar_type::IF_BLOCK: {
                block_format(file, (is_not_end_node*)node, tabsize, "if");
                break;
            }
            case grammar_type::WHILE_BLOCK: {
                block_format(file, (is_not_end_node*)node, tabsize, "while");
                break;
            }
            default: {
                NOREACH("Unexpeceted grammar treee node type %d", (int)type);
            }
            }
        } else {
            switch (type) {
            case grammar_type::VAR_NAME: {
                // 变量名节点
                auto varname = ((node_base_string_without_sons*)node)->value;
                fprintf(file, "%s", varname);
                break;
            }
            case grammar_type::NUMBER: {
                // 整型节点
                auto value = ((node_base_int_without_sons*)node)->value;
                fprintf(file, "%d", value);
                break;
            }
            case grammar_type::STRING: {
                // 字符串节点
                fprintf(
                    file, "%s", ((node_base_string_without_sons*)node)->value);
                break;
            }
            case grammar_type::FLOAT: {
                // 浮点型节点
                auto value = ((node_base_float_without_sons*)node)->value;
                fprintf(file, "%lf", value);
                break;
            }
            case grammar_type::LONG_INT: {
                // 长整型节点
                fprintf(
                    file, "%s", ((node_base_string_without_sons*)node)->value);
                break;
            }
            default: {
                NOREACH("Unexpeceted grammar treee node type %d.", (int)type);
            }
            }
        }
        fclose(file);
    }
}

namespace tools_out {
    void style() {
        for (int i = 2; i < argc; ++i) {
            std::string filedata;
            utils::readcode(filedata, argv[i]);
            // 覆盖掉该文件的所有内容
            FILE* file = fopen(argv[i], "w");
            // 清空文件并通过生成语法树解析过滤掉所有个人风格然后再全部重新按照规则排列，是为了让代码风格更加统一
            compiler::grammar_lex lexer(filedata, shared_data);
            for (auto node = lexer.get_node(); node != nullptr;
                 node = lexer.get_node()) {
                tools_in::__style_file(file, node, 0);
            }
            fclose(file);
        }
    }
}
}
