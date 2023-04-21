/**
 * 编译且保存
 * 注：先编译再保存
 */

module;
#include <string>
export module brun;
import TVM;
import build;
import generated_params;

export namespace trc::tools {
namespace tools_in {
    void _brun(TVM_space::TVM* vm, const std::string& path) {
        _build(vm, path);
        vm->reload_data();
        vm->run_all();
    }
}

namespace tools_out {
    /**
     * @brief 编译运行，并将编译结果写入文件
     */
    void brun() {
        auto vm = new TVM_space::TVM;
        char * argv;
        while((argv=default_argv_parser.next()) != nullptr) {
            tools_in::_brun(vm, argv);
        }
        delete vm;
    }
}
}
