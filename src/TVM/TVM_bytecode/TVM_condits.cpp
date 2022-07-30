#include <TVM/TRE.h>
#include <TVM/TVM.h>
#include <TVM/types/trc_string.h>
#include <base/Error.h>
#include <base/trcdef.h>
#include <language/error.h>

namespace trc::TVM_space {
void TVM::EQUAL() {
    def::OBJ secondv = pop();
    top() = top()->operator==(secondv);
}

void TVM::UNEQUAL() {
    def::OBJ secondv = pop();
    top() = top()->operator!=(secondv);
}

void TVM::GREATER_EQUAL() {
    def::OBJ secondv = pop();
    top() = top()->operator>=(secondv);
}

void TVM::LESS_EQUAL() {
    def::OBJ secondv = pop();
    top() = top()->operator<=(secondv);
}

void TVM::LESS() {
    def::OBJ secondv = pop();
    top() = top()->operator<(secondv);
}

void TVM::GREATER() {
    def::OBJ secondv = pop();
    top() = top()->operator>(secondv);
}

void TVM::IF_FALSE_GOTO(bytecode_index_t index) {
    auto firsti = (def::INTOBJ)(pop());
    if (!firsti->value)
        GOTO(index);
}

void TVM::ASSERT() {
    auto firsti = (def::INTOBJ)pop();
    auto secondi = (def::INTOBJ)pop();
    int tmp_i = firsti->value;
    if (!secondi->value) {
        switch (tmp_i) {
        case 1: {
            error_report(
                error::AssertError, language::error::asserterror_default);
            break;
        }
        case 2: {
            auto firsts = (def::STRINGOBJ)pop();
            error_report(error::AssertError, language::error::asserterror_user,
                firsts->c_str());
            break;
        }
        default:{
            error_report(error::ArgumentError, language::error::argumenterror,
                "assert", "one or two");
        }
        }
    }
}

void TVM::NOT() {
    top() = top()->operator!();
}

void TVM::AND() {
    def::OBJ secondv = pop();
    top() = top()->operator&&(secondv);
}

void TVM::OR() {
    def::OBJ secondv = pop();
    top() = top()->operator||(secondv);
}
}
