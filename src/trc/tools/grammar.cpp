/**
 * 递归输出语法树，为一个调试功能
 */

#include <Compiler/Compiler.h>
#include <Compiler/compiler_def.h>
#include <Compiler/grammar.h>
#include <base/utils/filesys.h>
#include <cstdio>
#include <string>
#include <tools.h>

using namespace trc::compiler;

namespace trc::tools {
namespace tools_in {
    static int tab_num = 0;
    static void outtab() {
        for (int i = 0; i < tab_num; ++i) {
            putchar('\t');
        }
    }

    /**
     * @brief 格式化输出语法树
     */
    void out(compiler::treenode* data) {
        outtab();
        puts("[");
        switch (data->type) {
        case grammar_type::TREE: {
            printf("%s", "tree");
            break;
        }
        case grammar_type::BUILTIN_FUNC: {
            printf("%s", "builtin_func");
            break;
        }
        case grammar_type::NUMBER: {
            auto tmp = (compiler::node_base_int_without_sons*)data;
            printf("%d", tmp->value);
            break;
        }
        default: {
            auto tmp = (compiler::data_node*)data;
            printf("%s", tmp->data);
        }
        }
        putchar(',');
        if (data->has_son()) {
            auto* tmp = (compiler::is_not_end_node*)data;
            for (auto& i : tmp->son)
                out(i);
        }
        outtab();
        putchar(']');
    }

    void __out_grammar(const std::string& path) {
        std::string file_data;
        utils::readcode(file_data, path);

        compiler::compiler_public_data compiler_data { path,
            &compiler::nooptimize_option };
        compiler::grammar_lex grammar_t(file_data, compiler_data);

        printf("From file %s:\n", path.c_str());

        compiler::treenode* tmp = grammar_t.get_node();
        size_t index = 0;
        while (tmp != nullptr) {
            printf("%zu:", index);
            out(tmp);
            putchar('\n');
            compiler::free_tree(tmp);
            tmp = grammar_t.get_node();
            index++;
        }
    }
}

namespace tools_out {
    void out_grammar() {
        for (int i = 2; i < argc; ++i)
            tools_in::__out_grammar(argv[i]);
    }
}
}
