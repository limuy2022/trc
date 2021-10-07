#include "../include/Error.h"
#include "../include/type.hpp"
#include "../include/Compiler/compile_share.h"
#include "../include/data.hpp"

using namespace std;

int what_type(const string &value) {
    /**
     * 判断传入的字符串实际代表的类型
     * return:一个字符串，类型
     */
    
    size_t n = value.length();
    if (('\'' == value[0] && '\'' == value[n - 1]) || (value[0] == '"' && value[n - 1] == '"'))
        return STRING_TICK;

    if (check_in(value[0], num)) {
        if (value.find('.') != string::npos) {
            // 小数
            if(n >= 12)
                return FLOAT_L_TICK;
            return FLOAT_TICK;
        }
        // 检查数据,前导零
        if(n >= 2 && value[0] == '0')
            send_error(SyntaxError, ("number " + to_type<string>(value) + " is incorrect.").c_str());
        if(n >= 12)
            return LONG_TICK;

        return INT_TICK;
    }
    if(check_in(value, const_values))
        return CONST_TICK;
    return VAR_TICK;
}
