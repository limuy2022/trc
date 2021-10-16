#ifndef TRC_UTILS_FILESYS_H
#define TRC_UTILS_FILESYS_H

#include "cfg.h"

using namespace std;

void listfiles(const string &dir, const string &file, vecs &fileList, vecs &dirList);

bool check_file_is(const string &path);

string path_last(const string &path, const string &last);

string import_to_path(const string &import_name);

string path_join(int n, ...);

string file_last_(const string &path);

#endif
