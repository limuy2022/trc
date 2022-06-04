#include "base/easter_eggs.h"
#include "language/language.h"
#include "base/trcdef.h"
#include "base/color.h"
#include <cstdio>

namespace trc::tools::tools_out {
void help() {
    puts(language::help::help_msg);
    color::green("\nVersion %.1f\n", def::version);
    color::green("github repo:%s\n", def::repo_github);
    color::green("gitee repo:%s\n", def::repo_gitee);
}
}
