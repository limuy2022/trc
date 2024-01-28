module;
#include <cstdio>
#include <cstdlib>
#include <libintl.h>
#include <source_location>
#include <string>
export module unreach;
import error;

namespace trc {
export void unreach(const std::string& error_msg,
    const std::source_location& source_info = std::source_location::current()) {
    fprintf(stderr,
        "%s\nFatal error in function \"%s\" file %s line "
        "%u\n%s",
        error_msg.c_str(), source_info.function_name(), source_info.file_name(),
        source_info.line(), gettext(error::noreach));
    exit(EXIT_FAILURE);
}
}
