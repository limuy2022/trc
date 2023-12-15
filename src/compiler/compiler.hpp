#pragma once

#include "parser.tab.hpp"
#include <string>

namespace trc::TVM_space {
    class TVM_static_data;
}

namespace trc::compiler {
class compiler_public_data;

class compiler_option;

class compiler {
public:
    compiler();

    // Run the parser on file F.  Return 0 on success.
    int parse(const compiler_option& option, const std::string& f, TVM_space::TVM_static_data* vm);
    int parse(const compiler_option& option, const std::string&filename, FILE* f, TVM_space::TVM_static_data* vm);
    // Whether to generate parser debug traces.
    bool trace_parsing = false;

    // Handling the scanner.
    void scan_begin(const std::string& path);
    void scan_begin(FILE* path);
    void scan_end();
    // Whether to generate scanner debug traces.
    bool trace_scanning = false;
    // The token's location used by the scanner.
    yy::location location;

private:
    FILE* open_file(const std::string& file);

    compiler_public_data* compile_data = nullptr;
};
}

// Give Flex the prototype of yylex we want ...
#define YY_DECL yy::parser::symbol_type yylex(trc::compiler::compiler& drv)
// ... and declare it for the parser's sake.
YY_DECL;
