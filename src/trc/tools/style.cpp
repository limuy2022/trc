#include <Compiler/token.hpp>
#include <base/utils/filesys.hpp>
#include <cstdio>
#include <string>
#include <tools.hpp>

namespace trc::tools {
static TVM_space::TVM_static_data vm;
static compiler::compiler_public_data shared_data(
    compiler::main_module, { true, false, true }, vm);

namespace tools_in { }

namespace tools_out {
    void style() {
        for (int i = 2; i < argc; ++i) {
            std::string filedata;
            utils::readcode(filedata, argv[i]);
            // 覆盖掉该文件的所有内容
            FILE* file = fopen(argv[i], "w");

            fclose(file);
        }
    }
}
}
