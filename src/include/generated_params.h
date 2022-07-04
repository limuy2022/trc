/**
 * 控制生成参数，如生成编译器参数
 */

#include <Compiler/Compiler.h>

namespace trc::tools {
/**
 * @brief 生成编译器参数
 * @warning 记得释放内存，使用delete
 */
compiler::compiler_option* generate_compiler_params();
}
