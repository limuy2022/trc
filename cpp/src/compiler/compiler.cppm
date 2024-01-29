module;
#include <string>
export module compiler;
import Error;
import compiler_def;

namespace trc::TVM_space {
class TVM_static_data;
}

namespace trc::compiler {
class compiler {
public:
    compiler();

    // Run the parser on file F.  Return 0 on success.
    int parse(const compiler_option& option, const std::string& f,
        TVM_space::TVM_static_data* vm);
    int parse(const compiler_option& option, const std::string& filename,
        FILE* f, TVM_space::TVM_static_data* vm);
    // Whether to generate parser debug traces.
    bool trace_parsing = false;

    // Handling the scanner.
    void scan_begin(const std::string& path);
    void scan_begin(FILE* path);
    void scan_end();
    // Whether to generate scanner debug traces.
    bool trace_scanning = false;
    // The token's location used by the scanner.

private:
    FILE* open_file(const std::string& file);

    compiler_public_data* compile_data = nullptr;
};
}

namespace trc::compiler {
compiler::compiler() = default;

int compiler::parse(const compiler_option& option, const std::string& f,
    TVM_space::TVM_static_data* vm) {
}

int compiler::parse(const compiler_option& option, const std::string& filename,
    FILE* f, TVM_space::TVM_static_data* vm) {
}

FILE* compiler::open_file(const std::string& file) {
}
}
