#include <tools.h>

namespace trc::tools {
namespace tools_in {
    void __style_file(const char* filepath) {
    }
}

namespace tools_out {
    void style() {
        for (int i = 2; i < argc; ++i) {
            tools_in::__style_file(argv[i]);
        }
    }
}
}
