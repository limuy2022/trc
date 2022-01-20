#include "base/easter_eggs.h"
#include "language/language.h"
#include <cstdio>

namespace trc::tools::tools_out {
void help() {
    puts(language::help::help_msg);
}
}
