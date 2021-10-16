#include "../../include/read.h"
#include "../../include/filesys.h"
#include "../../include/Compiler/Compiler.h"
#include "../../include/Error.h"
#include "../../include/share.h"
#include "../../include/TVM/TRE.h"
#include "../../include/TVM/TVM.h"
#include "../../include/TVM/bignum.h"
#include "../../include/filesys.h"
#include "../../include/TVM/int.h"
#include "../../include/TVM/float.h"
#include "../../include/TVM/string.h"
#include "../../include/memory/objs_pool.hpp"

using namespace std;

void TVM::LOAD_INT(const short &index) {
    /**
     * 加载整型变量到栈
     */
    int value = static_data.const_i[index];
    if(INT_CACHE_BEGIN <= value && value <= INT_CACHE_END) {
        // 处在缓存范围中

        // 减去负数偏移量
        push(TVM_share::int_cache[value - INT_CACHE_BEGIN]);
        return;
    }
    firsti = mem_control -> pool_ ->int_pool -> trcmalloc();
    firsti -> value = value;
    push(firsti);
}


void TVM::LOAD_FLOAT(const short &index) {
    /**
     * 加载浮点型变量到栈
     */
    firstf = mem_control -> pool_ ->float_pool -> trcmalloc();
    firstf -> value = static_data.const_f[index];
    push(firstf);
}

void TVM::LOAD_STRING(const short &index) {
    /**
     * 加载字符串变量到栈
     */
    firsts = mem_control -> pool_ ->str_pool -> trcmalloc();
    firsts->operator=(static_data.const_s[index]);
    push(firsts);
}

void TVM::LOAD_LONG(const short &index) {
    /**
     * 加载大整数变量到栈
     */
    firstl = mem_control -> pool_ ->long_pool -> trcmalloc();
    firstl -> operator=(static_data.const_long[index]);
    push(firstl);
}

void TVM::LOAD_ARRAY(const short&index) {
    
}

/* 什么都不做 */
void TVM::NOP() {}

void TVM::GOTO(const short &index) {
    /**
     * 跳转到行
     */
    run_env::lines[name] = static_data.const_i[index] - 1;
}

static void fix_path(string &path) {
    /**
     * 检查路径是否正确
     * 如果路径不正确将路径转换成正确的，否则报错
     */

    if(check_file_is(path)) {
        return;
    }
    if(check_file_is(path_join(2, "TVM/packages/base_support", path))) {
        return;
    }
    if(check_file_is(path_join(2, "TVM/packages/self_support", path))) {
        return;
    }
    if(check_file_is(path_join(2, "TVM/packages/other_support", path))) {
        return;
    }

    send_error(ModuleNotFoundError, path);
}

void TVM::IMPORT() {
    /**
     * 模块导入
     * 模块导入规则：
     * 首先检查是否为自定义模块
     * 其次检查是否为由系统原生支持的模块(由c++语言支持，定义在importlib动态链接库中)
     * 然后检查TVM/packages/self_support(由语言本身支持的模块)
     * 最后检查TVM/packages/other_support(由第三方支持的模块)
     */

    TVM *module;
    string codes, file_path, name_s;
    pop(firsti);
    int n = firsti->value;
    for (int i = 0; i < n; ++i) {
        pop(firsts);
        name_s = firsts->c_str();
        module = create_TVM(name_s);
        file_path = import_to_path(name_s) + ".tree";
        fix_path(file_path);

        readcode(codes, file_path);

        Compiler(module, codes);
        modules[name_s] = module;
        module->run();
    }
}
