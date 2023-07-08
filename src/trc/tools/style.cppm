module;
#include <cstdio>
#include <string>
export module style;
import token;
import filesys;
import base;
import TVM_data;
import compiler_def;
import Compiler;
import generated_params;
import cmdparser;
import basic_def;

namespace trc::tools {
static TVM_space::TVM_static_data vm;
static compiler::compiler_public_data shared_data(
    compiler::main_module, { true, false, true }, vm);

namespace tools_in { }

namespace tools_out {
    /**
     * @brief 格式化代码文件
     */
    export void style() {
        for (int i = cmdparser::optind + 1; i < tools::argc; ++i) {
            std::string filedata;
            utils::readcode(filedata, argv[i]);
            // 覆盖掉该文件的所有内容
            FILE* file = fopen(argv[i], "w");
            fclose(file);
        }
    }
}
}
