#include <TVM/TRE.h>
#include <TVM/TVM.h>

namespace trc::TVM_space {
void TVM::ADD() {
    def::OBJ secondv = pop();
    top() = top()->operator+(secondv);
}

void TVM::SUB() {
    def::OBJ secondv = pop();
    top() = top()->operator-(secondv);
}

void TVM::MUL() {
    def::OBJ secondv = pop();
    top() = top()->operator*(secondv);
}

void TVM::DIV() {
    def::OBJ secondv = pop();
    top() = top()->operator/(secondv);
}

void TVM::ZDIV() {
    def::OBJ secondv = pop();
    top() = top()->zdiv(secondv);
}

void TVM::POW() {
    def::OBJ secondv = pop();
    top() = top()->pow_(secondv);
}

void TVM::MOD() {
    def::OBJ secondv = pop();
    top() = top()->operator%(secondv);
}
}
