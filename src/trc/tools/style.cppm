module;
#include <cstdio>
#include <string>
export module style;
import filesys;
import base;
import TVM_data;
import compiler_def;
import generated_params;
import cmdparser;
import basic_def;
import compile_env;
import color;
import help;

namespace trc::tools {
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
