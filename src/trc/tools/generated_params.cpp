#include <Compiler/Compiler.hpp>
#include <gflags/gflags.h>

// 这里是编译器参数
// 行号表参数
DEFINE_bool(noline_number_table, false, "Controls whether a line number.");
// 是否优化
DEFINE_bool(
    optimize, false, "Controls whether optimization code is generated.");
// 是否进行常量折叠
DEFINE_bool(no_const_fold, false,
    "Controls whether optimization includes costant fold.");

namespace trc::tools {
compiler::compiler_option generate_compiler_params() {
    // 取反是为了更符合使用习惯
    return compiler::compiler_option { !FLAGS_noline_number_table,
        FLAGS_optimize, !FLAGS_no_const_fold };
}
}
