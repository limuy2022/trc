/**
 * 声明编译中的优化函数
 */
#ifndef TRC_INCLUDE_OPTIMIZE_H
#define TRC_INCLUDE_OPTIMIZE_H

#include <string>
#include <map>

using namespace std;

extern map<string, bool (*)(const int &, const int &)> optimize_condit;
extern map<string, int (*)(const int &, const int &)> optimize_number;

#endif
