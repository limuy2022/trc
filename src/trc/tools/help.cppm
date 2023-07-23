module;
#include <cstdio>
#include <cstring>
#include <format>
export module help;
import color;
import trcdef;
import cmdparser;
import basic_def;
import language;

static void output_optimze_msg() {
    puts("  --optimize,-o:optimize the code.");
}

static void output_line_table_msg() {
    puts("  --noline_number_table:don't genarate the line number table.");
}

static void output_const_fold_msg() {
    puts("  --no_const_fold:don't fold the constant value.");
}

static void argvs_output() {
    puts("There are the flags:\n");
}

namespace {
const char* build_msg
    = "build [file name]:Build a tree file.Turn it from tree to ctree.";
const char* run_msg = "run [file name]:Run a tree file or a ctree file.";
const char* help_msg
    = "help [instruction name]:Print the help message of this instruction.";
const char* dis_msg
    = "dis [file name]:Discompile a ctree file/a tree file to vm's opcode";
const char* brun_msg = "brun [file name]:Build a file and run it";
const char* token_msg = "token [file name]:Turn a tree file to tokens";
const char* style_msg = "style [file name/dir name]:Format a tree file/all "
                        "tree files in the direction";
}

namespace trc::tools::tools_out {
/**
 * @brief Trc命令行操作帮助文档
 */
export void help() {
    if (tools::argc == 2) {
        // output common msg
        puts(language::help::help_msg);
        color::green(std::format("\nVersion {}\n", def::version));
        color::green(std::format("github repo:{}\n", def::repo_github));
        color::green(std::format("gitee repo:{}\n", def::repo_gitee));
        puts("\nThere are the tools(all tools not only support one file):\n");
        puts(build_msg);
        puts(run_msg);
        puts(help_msg);
        puts(dis_msg);
        puts(brun_msg);
        puts(token_msg);
        puts(style_msg);
        return;
    }
    for (int i = cmdparser::optind + 1; i < tools::argc; ++i) {
        if (!strcmp(tools::argv[i], "run")) {
            puts(run_msg);
            argvs_output();
            output_line_table_msg();
            output_optimze_msg();
            output_const_fold_msg();
        } else if (!strcmp(tools::argv[i], "brun")) {
            puts(brun_msg);
            argvs_output();
            output_optimze_msg();
            output_line_table_msg();
            output_const_fold_msg();
        } else if (!strcmp(tools::argv[i], "style")) {
            puts(style_msg);
        } else if (!strcmp(tools::argv[i], "build")) {
            puts(build_msg);
            argvs_output();
            output_optimze_msg();
            output_line_table_msg();
            output_const_fold_msg();
        } else if (!strcmp(tools::argv[i], "help")) {
            puts(help_msg);
        } else if (!strcmp(tools::argv[i], "dis")) {
            puts(dis_msg);
            argvs_output();
            puts("Notice:If you use it discompile a tree file.There are some "
                 "options:");
            output_optimze_msg();
            output_const_fold_msg();
        } else if (!strcmp(tools::argv[i], "token")) {
            puts(token_msg);
        } else {
            color::red(std::format(
                "The tool \"{}\" is not found.Please check your input.\n",
                tools::argv[i]));
        }
        putchar('\n');
    }
}
}
