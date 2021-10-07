#include "../../include/TVM/TVM.h"
#include "../../include/TVM/built_in_func.h"
#include "../../include/share.h"
#include "../../include/TVM/int.h"

using namespace std;

typedef void (*TVM_BUILTINS_FUNC)(int argc, TVM *vm);

static TVM_BUILTINS_FUNC builtin_funcs[] {
    EXIT,
    PRINT,
    PRINTLN,
    INPUT,
    LENGTH,
    HELP,
    STRING_,
    INT_,
    BOOL_,
    FLOAT_,
    TYPE
};

void TVM::CALL_BUILTIN(const short &name) {
    /**
     * 执行内置函数
     */
    pop(firsti);
    int tmp = firsti->value;
    builtin_funcs[name](tmp, this);
}

void TVM::CALL_FUNCTION(const short &index) {
    /**
     * 执行自定义函数
     * 事实上是在虚拟机内创建帧，在帧中完成字节码运行
     * 帧就相当于一个小环境
     */

    func_ *fast = static_data.funcs[static_data.const_name[index]];
    run_func_str = fast->name;
    frame_ *frame_var = new frame_;
    frames.push(frame_var);
    // 调用函数
    this->run_func(fast->bytecodes, LINE_NOW);
}

void TVM::FREE_FUNCTION() {
    /**
    * 还原到调用函数前
    */
    
    delete frames.top();
    frames.pop();
}

void TVM::CALL_METHOD(const short&index) {
    
}

