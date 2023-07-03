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
import cmdparser;

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
        for(int i = cmdparser::optind + 1; i < tools::argc; ++i) {
            tools_in::_brun(vm, tools::argv[i]);
        }
        delete vm;
    }
}
}
