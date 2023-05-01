module;
#include <language/error.hpp>
module TVM;
import TRE;
import trcdef;
import Error;

namespace trc::TVM_space {
void TVM::ADD() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator+(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror, "+",
            TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::SUB() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator-(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror, "-",
            TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::MUL() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator*(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror, "*",
            TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::DIV() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator/(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror, "/",
            TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::ZDIV() {
    def::OBJ secondv = pop();
    auto tmp = top()->zdiv(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror, "//",
            TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::POW() {
    def::OBJ secondv = pop();
    auto tmp = top()->pow_(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror, "**",
            TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::MOD() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator%(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror, "%",
            TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}
}
