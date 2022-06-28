/**
 * 声明编译中的优化函数
 */

#pragma once

#include <map>
#include <string>

namespace trc::compiler {
typedef int (*ip)(const int&, const int&);

typedef bool (*bp)(const int&, const int&);

extern std::map<std::string, bp> optimize_condit;
extern std::map<std::string, ip> optimize_number;
}
