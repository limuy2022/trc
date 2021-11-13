/**
 * 常量折叠：加减乘除模运算
 */

#include <map>
#include <string>
#include <cmath>
#include "Error.h"

using namespace std;

// 关于数字的常量折叠

typedef int (*ip)(const int &, const int &);

typedef bool (*bp)(const int &, const int &);

static int add(const int &a, const int &b) {
    return a + b;
}

static int sub(const int &a, const int &b) {
    return a - b;
}

static int mul(const int &a, const int &b) {
    return a * b;
}

static int zdiv_(const int &a, const int &b) {
    if (b) return a / b;
    send_error(ZeroDivError, to_string(b).c_str());
}

static int mod(const int &a, const int &b) {
    if (b) return a % b;
    send_error(ZeroDivError, to_string(b).c_str());
}

static int _pow(const int &a, const int &b) {
    return pow(a, b);
}

//关于布尔值的常量折叠

static bool equal_(const int &a, const int &b) {
    return a == b;
}

static bool unequal_(const int &a, const int &b) {
    return a != b;
}

static bool less_(const int &a, const int &b) {
    return a < b;
}

static bool greater_(const int &a, const int &b) {
    return a > b;
}

static bool greater_equal_(const int &a, const int &b) {
    return a >= b;
}

static bool less_equal_(const int &a, const int &b) {
    return a <= b;
}

static bool and_(const int &a, const int &b) {
    return a && b;
}

static bool or_(const int &a, const int &b) {
    return a || b;
}

map<string, bp> optimize_condit = {
        {"==",  equal_},
        {"!=",  unequal_},
        {"<",   less_},
        {">",   greater_},
        {">=",  greater_equal_},
        {"<=",  less_equal_},
        {"and", and_},
        {"or",  or_},
};

map<string, ip> optimize_number = {
        {"+",  add},
        {"-",  sub},
        {"*",  mul},
        {"//", zdiv_},
        {"%",  mod},
        {"**", _pow},
};
