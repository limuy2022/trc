#ifndef TRC_INCLUDE_CTREE_LOADER_H
#define TRC_INCLUDE_CTREE_LOADER_H

using namespace std;

class TVM;

void loader_ctree(TVM *vm, const string &path);

void save_ctree(TVM *vm, const string &path);

bool is_magic(const string &path);

#endif
