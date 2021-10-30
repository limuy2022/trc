#include <string>
#include "TVM/TVM.h"
#include "data.hpp"
#include "Error.h"
#include "cfg.h"
#include "TVM/TRE.h"

using namespace std;

#define NAMEERROR(name)\
if(!map_check_in_first(var_names, name)) {\
    send_error(NameError, (name));\
}

void TVM::CHANGE_VALUE(const short &index) {
    /**
     * 改变变量的值
     */

    const string& name_v = static_data.const_name[index];
    NAMEERROR(name_v.c_str());
    pop(firstv);
    var_names[name_v] = firstv;
}

void TVM::DEL() {
    /**
     * 删除变量
     */
    
    pop(firsti);
    int n = firsti->value;
    for (int i = 0; i < n; ++i) {
        pop(firsts);
        NAMEERROR(firsts->c_str());
        var_names.erase(string(firsts->c_str()));
    }
}

void TVM::LOAD_NAME(const short &index) {
    /**
     * 读取变量的值
     */
    const string &name(static_data.const_name[index]);
    NAMEERROR(name.c_str());
    push(var_names[name]);
}

void TVM::STORE_NAME(const short &index) {
    /**
     * 新建变量
     */
    pop(firstv);
    var_names[static_data.const_name[index]] = firstv;
}

void TVM::STORE_LOCAL(const short &name) {
    /**
     * 生成函数局部变量
     */
    frame_ *fast = frames.top();
    pop(firstv);
    fast->var_names[static_data.const_name[name]] = firstv;
}

void TVM::LOAD_LOCAL(const short &name) {
    /**
     * 读取函数局部变量
     */
    const string &s_name = static_data.const_name[name];
    NAMEERROR(s_name.c_str());
    push(frames.top()->var_names[s_name]);
}

void TVM::CHANGE_LOCAL(const short &index) {
    /**
     * 改变局部变量的值
     */

    const string &name_v = static_data.const_name[index];
    NAMEERROR(name_v.c_str());
    pop(firstv);
    frames.top()->var_names[name_v] = firstv;
}

void TVM::DEL_LOCAL() {
    /**
     * 删除局部变量
     */
    pop(firsti);
    frame_ *fast = frames.top();
    int n = firsti->value;
    for (int i = 0; i < n; ++i) {
        pop(firsts);
        NAMEERROR(firsts->c_str());
        fast->var_names.erase(firsts->c_str());
    }
}
