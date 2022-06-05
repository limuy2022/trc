#include <TVM/TVM.h>
#include <TVMbase/TRE.h>
#include <TVMbase/types/string.h>
#include <base/Error.h>
#include <base/trcdef.h>
#include <language/error.h>

namespace trc::TVM_space {
void TVM::EQUAL() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator==(secondv));
}

void TVM::UNEQUAL() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator!=(secondv));
}

void TVM::GREATER_EQUAL() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator>=(secondv));
}

void TVM::LESS_EQUAL() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator<=(secondv));
}

void TVM::LESS() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator<(secondv));
}

void TVM::GREATER() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator>(secondv));
}

void TVM::IF_FALSE_GOTO(short index) {
    firsti = (def::INTOBJ)(pop());
    if (!firsti->value)
        GOTO(index);
}

void TVM::ASSERT() {
    firsti = (def::INTOBJ)pop();
    secondi = (def::INTOBJ)pop();
    int tmp_i = firsti->value;
    if (!secondi->value) {
        if (tmp_i > 2)
            error_report(error::ArgumentError,
                language::error::argumenterror, "assert",
                "one or two");

        switch (tmp_i) {
        case 1: {
            error_report(error::AssertError,
                language::error::asserterror_default);
            break;
        }
        case 2: {
            firsts = (def::STRINGOBJ)pop();
            error_report(error::AssertError,
                language::error::asserterror_user,
                firsts->c_str());
            break;
        }
        }
    }
}

void TVM::NOT() {
    push(pop()->operator!());
}

void TVM::AND() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator&&(secondv));
}

void TVM::OR() {
    secondv = pop();
    firstv = pop();
    push(firstv->operator||(secondv));
}
}
