/**
 * 声明编译中的优化函数
 */
#pragma once

#include <map>
#include <string>

using namespace std;

namespace trc {
    namespace compiler {
        typedef int (*ip)(const int &, const int &);

        typedef bool (*bp)(const int &, const int &);
        
        extern map<string, bp> optimize_condit;
        extern map<string, ip> optimize_number;
    }
}
