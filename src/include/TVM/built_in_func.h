#ifndef TRC_INCLUDE_TVM_BUILT_IN_FUNC_H
#define TRC_INCLUDE_TVM_BUILT_IN_FUNC_H

class TVM;

void LENGTH(int argc, TVM *vm);

void HELP(int argc, TVM *vm);

void EXIT(int argc, TVM *vm);

void PRINT(int argc, TVM *vm);

void PRINTLN(int argc, TVM *vm);

void INPUT(int argc, TVM *vm);

void INT_(int argc, TVM *vm);

void FLOAT_(int argc, TVM *vm);

void STRING_(int argc, TVM *vm);

void BOOL_(int argc, TVM *vm);

void TYPE(int argc, TVM *vm);

#endif
