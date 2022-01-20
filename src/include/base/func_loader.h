#pragma once

#include "base/dll.h"
#include <array>
#include <map>
#include <string>

namespace trc::loader {
TRC_base_api extern std::array<std::string, 11> num_func;
TRC_base_api extern std::map<std::string, short> func_num;
}
