#pragma once

#include "dll.h"

namespace language {
    namespace error {
        TRC_language_api extern const char * error_map[];
        TRC_language_api extern const char * error_from;
        TRC_language_api extern const char * error_in_line;
    }
    namespace help {
        TRC_language_api extern const char * help_msg;
    }
    namespace TVM {
        TRC_language_api extern const char *type_change_error_msg;
        TRC_language_api extern const char *oper_not_def_error_msg;
    }
    namespace trc {
        TRC_language_api extern const char* mode_not_found;
    }
    namespace tdb {
        TRC_language_api extern const char * var;
        TRC_language_api extern const char* not_defined;
        TRC_language_api extern const char* start_tip;
        TRC_language_api extern const char* instruction;
    }
}
