#include "read.h"
#include "filesys.h"
#include "Compiler/Compiler.h"
#include "Error.h"
#include "share.h"
#include "TVM/TRE.h"
#include "TVM/TVM.h"
#include "filesys.h"
#include "cfg.h"
#include "memory/objs_pool.hpp"
#include "TVM/lib.h"
#include "data.hpp"

using namespace std;
using namespace memory;
using namespace TVM_temp;

void TVM::LOAD_INT(const short &index) {
    /**
     * 加载整型变量到栈
     */
    int value = static_data.const_i[index];
    if (INT_CACHE_BEGIN <= value && value <= INT_CACHE_END) {
        // 处在缓存范围中

        // 减去负数偏移量
        push(TVM_share::int_cache[value - INT_CACHE_BEGIN]);
        return;
    }
    firsti = global_objs_pool->int_pool->trcmalloc();
    firsti->value = value;
    push(firsti);
}

void TVM::LOAD_MAP(const short &index) {
    /**
     * 加载map型变量到栈，参数含义是有几个参数
     */
}

void TVM::LOAD_FLOAT(const short &index) {
    /**
     * 加载浮点型变量到栈
     */
    firstf = global_objs_pool->float_pool->trcmalloc();
    firstf->value = static_data.const_f[index];
    push(firstf);
}

void TVM::LOAD_STRING(const short &index) {
    /**
     * 加载字符串变量到栈
     */
    firsts = global_objs_pool->str_pool->trcmalloc();
    firsts->operator=(static_data.const_s[index]);
    push(firsts);
}

void TVM::LOAD_LONG(const short &index) {
    /**
     * 加载大整数变量到栈
     */
    firstl = global_objs_pool->long_pool->trcmalloc();
    firstl->operator=(static_data.const_long[index]);
    push(firstl);
}

void TVM::LOAD_ARRAY(const short &index) {

}

/* 什么都不做 */
void TVM::NOP() {}

void TVM::GOTO(const short &index) {
    /**
     * 跳转到行
     */
    run_env::lines[name] = static_data.const_i[index] - 1;
}

static void fix_path(const string &path) {
    /**
     * 检查路径是否正确
     * 如果路径不正确将路径转换成正确的，否则报错
     */

    if (check_file_is(path)) {
        return;
    }
    if (check_in_array(cpp_libs::names, path, cpp_libs::libs_num)) {
        return;
    }
    if (check_file_is(path_join(2, "TVM/packages/self_support", path))) {
        return;
    }
    if (check_file_is(path_join(2, "TVM/packages/other_support", path))) {
        return;
    }

    send_error(ModuleNotFoundError, path.c_str());
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
