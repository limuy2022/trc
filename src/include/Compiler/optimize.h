/**
 * 声明编译中的优化函数
 */
#ifndef TRC_INCLUDE_OPTIMIZE_H
#define TRC_INCLUDE_OPTIMIZE_H

extern std::map<std::string, bool (*)(const int &, const int &)> optimize_condit;
extern std::map<std::string, int (*)(const int &, const int &)> optimize_number;

#endif
