#include "TVM/TVM.h"
#include "Error.h"
#include "cfg.h"
#include "TVM/TRE.h"

using namespace std;

void TVM::EQUAL() {
    /**
     * 判断相等，假设两端类型相等
     * 注意：如果不相等，将会在编译期间进行强制转换，不能通过者将会报出类型错误
     */
    
    pop(secondv);
    pop(firstv);
    push(firstv->operator==(secondv));
}

void TVM::UNEQUAL() {
    /**
     * 判断不相等
     */
    pop(secondv);
    pop(firstv);
    push(firstv->operator!=(secondv));
}

void TVM::GREATER_EQUAL() {
    /**
     * 判断大于等于
     */
    pop(secondv);
    pop(firstv);
    push(firstv->operator>=(secondv));
}

void TVM::LESS_EQUAL() {
    /**
     * 判断小于等于
     */
    pop(secondv);
    pop(firstv);
    push(firstv->operator<=(secondv));
}

void TVM::LESS() {
    /**
     * 判断小于
     */
    
    pop(secondv);
    pop(firstv);
    push(firstv->operator < (secondv));
}

void TVM::GREATER() {
    /**
     * 判断大于
     */
    pop(secondv);
    pop(firstv);
    push(firstv->operator>(secondv));
}


void TVM::IF_FALSE_GOTO(const short &index) {
    /**
     * 如果为否，跳转代码
     */

    pop(firsti);
    if (!firsti->value)
        GOTO(index);
}


void TVM::ASSERT() {
    /**
     * 断言，判断表达式是否为假
     */

    pop(firsti);
    pop(secondi);
    int tmp_i = firsti->value;
    if (!secondi->value) {
        if (tmp_i > 2)
            send_error(ArgumentError, "assert needs one or two arguments.");

        switch (tmp_i) {
            case 1:
                send_error(AssertError, "assert.");
            case 2:
                pop(firsts);
                send_error(AssertError, firsts->c_str());
        }
    }
}


void TVM::NOT() {
    pop(firstv);
    push(firstv->operator!());
}


void TVM::AND() {
    pop(secondv);
    pop(firstv);
    push(firstv->operator&&(secondv));
}

void TVM::OR() {
    pop(secondv);
    pop(firstv);
    push(firstv->operator||(secondv));
}
