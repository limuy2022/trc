#include <base/color.hpp>
#include <base/trcdef.hpp>
#include <cstdio>
#include <language/language.hpp>

namespace trc::tools::tools_out {
void help() {
    puts(language::help::help_msg);
    color::green("\nVersion %s\n", def::version);
    color::green("github repo:%s\n", def::repo_github);
    color::green("gitee repo:%s\n", def::repo_gitee);
}
}
