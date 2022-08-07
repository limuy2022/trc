#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

std::string redefine_path(const std::string& p) {
    return "../tests/unittest/testdata/" + p;
}

FILE* fopen_with_check(const char* path, const char* mode) {
    FILE* file = fopen(path, mode);
    if (file == nullptr) {
        fprintf(stderr,
            "The test data has been lost.Unable to "
            "continue testing.\n");
        exit(EXIT_FAILURE);
    }
    return file;
}
