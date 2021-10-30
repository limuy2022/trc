/**
 * 编译器所有供外调的函数
 */

#ifndef TRC_INCLUDE_COMPILER_COMPILER_H
#define TRC_INCLUDE_COMPILER_COMPILER_H

#include "cfg.h"

using namespace std;

class TVM;

class treenode;

void pre(vecs &result, const string &codes);

void pre_token_2(vecs2d &tokens);

vecs2d final_token(const vecs &codes);

void grammar(vector<treenode *> &result_return, const vecs2d &codes);

void Compiler(TVM *vm, const string &codes);

void check(const vecs2d &code);

void token(vecs2d &result, const vecs &codes);

#endif
