#include <cmath>
#include "type.hpp"
#include "TVM/TVM.h"
#include "TVM/TRE.h"

using namespace std;
using namespace TVM_temp;

void TVM::ADD() {
    /**
     * 支持拼接对象：int，float，string
     */
    pop(secondv);
    pop(firstv);
    push(firstv->operator+(secondv));
}

void TVM::SUB() {
    /**
     * 支持对象：int， float
     */
    pop(secondv);
    pop(firstv);
    push(firstv->operator-(secondv));
}

void TVM::MUL() {
    /**
     * 支持对象：int，float，string
     */
    pop(secondv);
    pop(firstv);
    push(firstv->operator*(secondv));
}

void TVM::DIV() {
    /**
     * 支持对象：int， float
     */
    pop(secondv);
    pop(firstv);
    push(firstv->operator/(secondv));
}

void TVM::ZDIV() {
    /**
     * 支持对象：int， float
     */
    pop(secondv);
    pop(firstv);
    push(firstv->zdiv(secondv));
}

void TVM::POW() {
    pop(secondv);
    pop(firstv);
    push(firstv->pow_(secondv));
}

void TVM::MOD() {
    pop(secondv);
    pop(firstv);
    push(firstv->operator%(secondv));
}
