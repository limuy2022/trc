#include "../include/Error.h"
#include "../include/type.hpp"
#include "../include/TVM/TRE.h"
#include "../include/TVM/TVM.h"
#include "../include/TVM/bignum.h"
#include "../include/TVM/int.h"
#include "../include/TVM/string.h"
#include "../include/TVM/float.h"
#include "../include/memory/mem.h"
#include "../include/share.h"
#include "../include/memory/objs_pool.hpp"

#define STACK_ERROR_INFO "Stack is empty."

#define STACK_RUN_ERROR(stack_)\
if((stack_).empty()) {\
    send_error(RunError, STACK_ERROR_INFO);\
}

#define STACK_TYPE_ERROR(info) \
do{\
    send_error(TypeError, "error in "#info" stack.");\
}while(0)

using namespace std;

// 利用索引存放函数指针，实现O(1)复杂度的调用算法
// 不符合的地方用nullptr代替，算是以小部分空间换大部分时间
static NOARGV_TVM_METHOD TVM_RUN_CODE_NOARG_FUNC[] = {
    nullptr,
    TVM::ADD,
    TVM::NOP,
    TVM::SUB,
    TVM::MUL,
    TVM::DIV,
    nullptr,
    nullptr,
    nullptr,
    TVM::DEL,
    nullptr,
    nullptr,
    nullptr,
    TVM::IMPORT,
    TVM::POW,
    TVM::ZDIV,
    TVM::MOD,
    nullptr,
    nullptr,
    TVM::EQUAL,
    TVM::UNEQUAL,
    TVM::GREATER_EQUAL,
    TVM::LESS_EQUAL,
    TVM::LESS,
    TVM::GREATER,
    TVM::ASSERT,
    TVM::NOT,
    TVM::AND,
    TVM::OR,
    nullptr,
    nullptr,
    nullptr,
    TVM::FREE_FUNCTION,
    nullptr,
    TVM::DEL_LOCAL,
    nullptr,
    nullptr,
    nullptr
};

static ARGV_TVM_METHOD TVM_RUN_CODE_ARG_FUNC[] = {
    TVM::LOAD_INT,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    TVM::GOTO,
    TVM::STORE_NAME,
    TVM::LOAD_NAME,
    nullptr,
    TVM::LOAD_FLOAT,
    TVM::LOAD_STRING,
    TVM::CALL_BUILTIN,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    TVM::IF_FALSE_GOTO,
    TVM::CHANGE_VALUE,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    TVM::STORE_LOCAL,
    TVM::LOAD_LOCAL,
    TVM::CALL_FUNCTION,
    nullptr,
    TVM::CHANGE_LOCAL,
    nullptr,
    TVM::LOAD_LONG,
    TVM::LOAD_ARRAY,
    TVM::CALL_METHOD
};

TVM::TVM(const string &name, const float &ver_in):
    name(name),
    firsti(mem_control -> pool_ ->int_pool -> trcmalloc()),
    secondi(mem_control -> pool_ ->int_pool -> trcmalloc()),
    firstf(mem_control -> pool_ ->float_pool -> trcmalloc()),
    secondf(mem_control -> pool_ ->float_pool -> trcmalloc()),
    firsts(mem_control -> pool_ ->str_pool -> trcmalloc()),
    seconds(mem_control -> pool_ ->str_pool -> trcmalloc()),
    firstl(mem_control -> pool_ ->long_pool -> trcmalloc()), 
    secondl(mem_control -> pool_ ->long_pool -> trcmalloc()),
    mem_control(new gc_obj)
    {
    static_data.ver_ = ver_in;
    // 对比版本号，并拒绝版本号高于TVM的程序
    check_TVM();
    // 新建默认变量
    var_names["__name__"] = (OBJ)new trc_string(string("__main__"));
}

void TVM::check_TVM() const {
    /**
     * 检查TVM版本号是否正确
     * 不正确立刻退出程序
     */
    if (static_data.ver_ > version) {
        send_error(VersionError, to_string(static_data.ver_).c_str(), \
                                to_string(version).c_str());
    }
}

TVM::~TVM() {
    // 释放字节码和变量的值
    free_TVM(this);
    // 重置
    delete mem_control;
    run_env::lines.erase(name);
    for (const auto &i : static_data.funcs) {
        delete i.second;
    }
}

void TVM::run() {
    /**
     * 从头开始，执行所有字节码
     */

    string &parent = run_env::run_module;
    run_env::run_module = name;
    
    // 将整个程序当成一个函数执行
    this->run_func(static_data.byte_codes, 0);

    run_env::run_module = parent;
}

void TVM::run_func(vector<vector<short*> > &bytecodes_, int init_index) {
    /**
     * 执行局部字节码（函数）
     * init_index:代码运行初始化索引
     */

    // 原先的代码行数，用于还原索引
    int re_ = LINE_NOW;
    LINE_NOW = init_index;
    auto* pointer = &LINE_NOW;
    size_t size = bytecodes_.size();
    while (*pointer < size) {
        for (const auto &i: bytecodes_[*pointer])
            if(i[1] == -1)
                (this->*TVM_RUN_CODE_NOARG_FUNC[i[0]])();
            else
                (this->*TVM_RUN_CODE_ARG_FUNC[i[0]])(i[1]);
        (*pointer)++;
    }
    *pointer = re_;
}

void TVM::run_step() {
    /**
     * 单步执行，为tdb调试功能
     * 注意：此功能和run函数不能混用！
     */

    auto *pointer = &LINE_NOW;
    for (const auto &i : static_data.byte_codes[*pointer])
        if(i[1] == -1)
            (this->*TVM_RUN_CODE_NOARG_FUNC[i[0]])();
        else
            (this->*TVM_RUN_CODE_ARG_FUNC[i[0]])(i[1]);
    (*pointer)++;
}

void TVM::push(OBJ a) {
    stack_data.push(a);
}

void TVM::pop_value() {
    /**
     * 弹出栈顶的值
     * 注意：作为一个不可能被利用的值，在这里会被直接析构
     */
    delete stack_data.top();
    stack_data.pop();
}

TVM *create_TVM(const string &name) {
    TVM *vm = new TVM(name);
    run_env::set_module(name);
    return vm;
}

#undef STACK_ERROR_INFO
