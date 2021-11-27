/**
 * 常量折叠：加减乘除模运算
 */

#include <map>
#include <string>
#include <cmath>
#include "base/Error.h"
#include "Compiler/optimize.h"

using namespace std;

// 关于数字的常量折叠
namespace trc {
    namespace compiler {
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
            if (b) return a / b;
            error::send_error(error::ZeroDivError, to_string(b).c_str());
        }

        int mod(const int& a, const int& b) {
            if (b) return a % b;
            error::send_error(error::ZeroDivError, to_string(b).c_str());
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
    }
}
