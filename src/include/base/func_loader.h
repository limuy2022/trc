#pragma once

#include <array>
#include <base/library.h>
#include <map>
#include <string>

namespace trc::loader {
TRC_base_c_api std::array<const char*, 11> num_func;
TRC_base_c_api std::map<std::string, short> func_num;
}
