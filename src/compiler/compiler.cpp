#include "compiler.hpp"
#include <string>
import Error;
import compiler_def;

namespace trc::compiler {
compiler::compiler() = default;

int compiler::parse(const compiler_option& option, const std::string& f, TVM_space::TVM_static_data* vm) {
    location.initialize(&f);
    compile_data = new compiler_public_data(option, vm, location);
    scan_begin(f);
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();
    delete compile_data;
    compile_data = nullptr;
    return res;
}

int compiler::parse(const compiler_option& option, const std::string&filename,  FILE* f, TVM_space::TVM_static_data* vm) {
    location.initialize(&filename);
    compile_data = new compiler_public_data(option, *vm, location);
    scan_begin(f);
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();
    delete compile_data;
    compile_data = nullptr;
    return res;
}

FILE* compiler::open_file(const std::string& file) {
    auto f = fopen(file.c_str(), "r");
    if (f == nullptr) {
        compile_data->send_error(
            error::OpenFileError, language::error::openfileerror, file.c_str());
    }
    return f;
}
}
