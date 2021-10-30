#ifndef TRC_INCLUDE_READ_H
#define TRC_INCLUDE_READ_H

#include <string>

using namespace std;

void readcode(std::string &file_data, const std::string &path);

void readfile(std::string &file_data, const std::string &path, const int err, ...);

#endif
