#pragma once

#include <language/library.h>

namespace language {
/**
 * @brief 地域化初始化设置
 */
TRC_language_c_api void locale_init();

namespace help {
    TRC_language_c_api const char* help_msg;
}
namespace trc {
    TRC_language_c_api const char* mode_not_found;
}
namespace tdb {
    TRC_language_c_api const char* var;
    TRC_language_c_api const char* not_defined;
    TRC_language_c_api const char* start_tip;
    TRC_language_c_api const char* instruction;
}
}
