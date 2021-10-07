#ifndef TRC_UTILS_READ_H
#define TRC_UTILS_READ_H

#include <string>

using namespace std;

void readcode(string &file_data, const string &path);

void readfile(string &file_data, const string &path, const int err, ...);

#endif
