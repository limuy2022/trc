#include "TVM/TVM.h"
#include "TVMbase/TRE.h"

namespace trc::TVM_space {
void TVM::ADD() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator+(secondv));
}

void TVM::SUB() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator-(secondv));
}

void TVM::MUL() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator*(secondv));
}

void TVM::DIV() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator/(secondv));
}

void TVM::ZDIV() {
    secondv = pop();
    firstv = pop();
    push(firstv->zdiv(secondv));
}

void TVM::POW() {
    secondv = pop();
    firstv = pop();
    push(firstv->pow_(secondv));
}

void TVM::MOD() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator%(secondv));
}
}
