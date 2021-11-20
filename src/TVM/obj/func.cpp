#include "TVM/func.h"

using namespace std;

frame_::frame_() = default;

func_::func_(string name) :
        name(std::move(name)) {}
