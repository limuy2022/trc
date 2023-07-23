module;
module TVM;
import trc_string;
import Error;
import trcdef;
import TRE;
import TVMdef;
import language;

namespace trc::TVM_space {
void TVM::EQUAL() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator==(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror,
            "==", TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::UNEQUAL() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator!=(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror,
            "!=", TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::GREATER_EQUAL() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator>=(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror,
            ">=", TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::LESS_EQUAL() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator<=(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror,
            "<=", TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::LESS() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator<(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror, "<",
            TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
}

void TVM::GREATER() {
    def::OBJ secondv = pop();
    auto tmp = top()->operator>(secondv);
    if (tmp == nullptr) {
        error_report(error::OperatorError, language::error::operatorerror, ">",
            TVM_share::int_name[int(top()->gettype())],
            TVM_share::int_name[int(secondv->gettype())]);
    }
    top() = tmp;
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
        default: {
            error_report(error::ArgumentError, language::error::argumenterror,
                "assert", "one or two");
        }
        }
    }
}

void TVM::NOT() const {
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
