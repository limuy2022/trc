#include <TVM/TVMdef.h>

namespace trc::TVM_space {
symbol_form::~symbol_form() {
    free(vars);
}

symbol_form::symbol_form(size_t size)
    : vars((def::OBJ*)malloc(sizeof(def::OBJ) * size)) {
}

void symbol_form::reset(size_t size) {
    vars = (def::OBJ*)(realloc(vars, size * sizeof(def::OBJ)));
}
}
