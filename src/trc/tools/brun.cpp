/**
 * 编译且保存
 * 注：先编译再保存
 */

#include <TVM/TVM.hpp>
#include <string>
#include <tools.hpp>

namespace trc::tools {
namespace tools_in {
    void _brun(TVM_space::TVM* vm, const std::string& path) {
        _build(vm, path);
        vm->reload_data();
        vm->run_all();
    }
}

namespace tools_out {
    void brun() {
        auto vm = new TVM_space::TVM;
        for (int i = 2; i < argc; ++i) {
            tools_in::_brun(vm, argv[i]);
        }
        delete vm;
    }
}
}
