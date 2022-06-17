/**
 * 常量折叠：加减乘除模运算
 */

#include <Compiler/optimize.h>
#include <base/Error.h>
#include <cmath>
#include <language/error.h>
#include <map>
#include <string>

// 关于数字的常量折叠
namespace trc::compiler {
int add(const int& a, const int& b) {
    return a + b;
}

int sub(const int& a, const int& b) {
    return a - b;
}

int mul(const int& a, const int& b) {
    return a * b;
}

int zdiv_(const int& a, const int& b) {
    if (b)
        return a / b;
    error::send_error(error::ZeroDivError, language::error::zerodiverror,
        std::to_string(b).c_str());
}

int mod(const int& a, const int& b) {
    if (b)
        return a % b;
    error::send_error(error::ZeroDivError, language::error::zerodiverror,
        std::to_string(b).c_str());
}

int _pow(const int& a, const int& b) {
    return pow(a, b);
}

//关于布尔值的常量折叠

bool equal_(const int& a, const int& b) {
    return a == b;
}

bool unequal_(const int& a, const int& b) {
    return a != b;
}

bool less_(const int& a, const int& b) {
    return a < b;
}

bool greater_(const int& a, const int& b) {
    return a > b;
}

bool greater_equal_(const int& a, const int& b) {
    return a >= b;
}

bool less_equal_(const int& a, const int& b) {
    return a <= b;
}

bool and_(const int& a, const int& b) {
    return a && b;
}

bool or_(const int& a, const int& b) {
    return a || b;
}

std::map<std::string, bp> optimize_condit = {
    { "==", equal_ },
    { "!=", unequal_ },
    { "<", less_ },
    { ">", greater_ },
    { ">=", greater_equal_ },
    { "<=", less_equal_ },
    { "and", and_ },
    { "or", or_ },
};

std::map<std::string, ip> optimize_number = {
    { "+", add },
    { "-", sub },
    { "*", mul },
    { "//", zdiv_ },
    { "%", mod },
    { "**", _pow },
};
}
