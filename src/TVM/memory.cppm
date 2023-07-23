module;
#include <map>
export module TVM.memory;
import base;
import trc_float;
import trc_flong;
import trc_int;
import trc_long;
import trc_string;
import objs_pool;
import TVM;
import memory;
import TRE;

namespace trc::TVM_space {
class TVM_dyna_data;
/**
 * 对对象池的基本封装，对象池面向全体对象
 * 而这个类面向TVM用于保存基础类型
 */
export struct objs_pool_TVM {
    memory::objs_pool<types::trc_int> int_pool;
    memory::objs_pool<types::trc_float> float_pool;
    memory::objs_pool<types::trc_string> str_pool;
    memory::objs_pool<types::trc_long> long_pool;
    memory::objs_pool<types::trc_flong> flong_pool;

    template<typename ...T>
    types::trc_int* MALLOCINT(const T&...argv) {
        return int_pool.trcmalloc(argv...);
    }

    template<typename ...T>
    types::trc_float* MALLOCFLOAT(const T&...argv) {
        return float_pool.trcmalloc(argv...);
    }

    template<typename ...T>
    types::trc_string* MALLOCSTRING(const T&...argv) {
        return str_pool.trcmalloc(argv...);
    }

    template<typename ...T>
    types::trc_long* MALLOCLONG(const T&...argv) {
        return long_pool.trcmalloc(argv...);
    }

    template<typename ...T>
    types::trc_flong* MALLOCFLONG(const T&...argv) {
        return flong_pool.trcmalloc(argv...);
    }
}* global_objs_pool;

/**
 * @brief 删除TVM中的栈信息，常量池，字节码
 * @warning 会保留变量信息（tshell）
 */
export void free_TVM(TVM* vm) {
    vm->static_data.byte_codes.clear();
    vm->static_data.const_i.clear();
    vm->static_data.const_f.clear();

    while (vm->dyna_data.frames_top_str != vm->dyna_data.frames - 1) {
        vm->dyna_data.frames_top_str->free_func();
        vm->dyna_data.frames_top_str--;
    }
    vm->static_data.global_symbol_table_size = 1;
    vm->dyna_data.reset_global_symbol_table(1);
}

export void TVM_quit_mem() {
    delete TVM_share::true_;
    delete TVM_share::false_;
    delete global_objs_pool;
}

export void init_mem() {
    global_objs_pool = new objs_pool_TVM;
    // 布尔值初始化
    TVM_share::true_ = new types::trc_int(1),
    TVM_share::false_ = new types::trc_int(0);
    // 整型缓存初始化，-5~256
    for (int i = INT_CACHE_BEGIN; i <= INT_CACHE_END; ++i)
        TVM_share::int_cache[i - INT_CACHE_BEGIN].value = i;
}
}
