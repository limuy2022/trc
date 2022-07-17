#include <Compiler/Compiler.h>
#include <gflags/gflags.h>

// 这里是编译器参数
// 行号表参数
DEFINE_bool(noline_number_table, false, "Controls whether a line number");
// 是否优化
DEFINE_bool(optimize, false, "Controls whether optimization code is generated");

namespace trc::tools {
compiler::compiler_option generate_compiler_params() {
    return compiler::compiler_option { FLAGS_noline_number_table,
        FLAGS_optimize };
}
}
