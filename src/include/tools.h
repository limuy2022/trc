/**
 * 声明trc定义的所有外调tools函数
 * tools格式：由一个供调用的函数以及一个函数依赖的基本实现
 * 例如：clean,__clean
 */

#ifndef TRC_INCLUDE_TOOLS_H
#define TRC_INCLUDE_TOOLS_H

#include <string>

using namespace std;

class TVM;

void __brun(TVM *vm, const string &path);

int brun(int argc, char *argv[]);

void __build(TVM* vm, const string& path);

int build(int argc, char *argv[]);

void __all(TVM* vm, const string& path);

int all(int argc, char *argv[]);

void __clean(const string &path);

int clean(int argc, char *argv[]);

void __crun(TVM *vm_run, const string &path);

int crun(int, char *argv[]);

void __dis(TVM *vm, const string &file_path);

int dis(int argc, char *argv[]);

void __out_grammar(const string &path);

int out_grammar(int argc, char *argv[]);

void __run(TVM *vm, const string &path);

int run(int argc, char *argv[]);

void __out_token(const string &path);

int out_token(int argc, char *argv[]);

int tshell();

int tdb();

#endif
