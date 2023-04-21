module;
#include <cstdio>
#include <language/language.hpp>
export module help;
import color;
import trcdef;

namespace trc::tools::tools_out {
/**
 * @brief Trc命令行操作帮助文档
 */
export void help() {
    puts(language::help::help_msg);
    color::green("\nVersion %s\n", def::version);
    color::green("github repo:%s\n", def::repo_github);
    color::green("gitee repo:%s\n", def::repo_gitee);
}
}
