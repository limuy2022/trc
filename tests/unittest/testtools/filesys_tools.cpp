#include <string>
#include <cstdio>
#include <cstdlib>

std::string redefine_path(const std::string& p) {
    return "../tests/unittest/testdata/" + p;
}

FILE* fopen_with_check(const char* path, const char* mode) {
    FILE*file = fopen(path, mode);
    if(file == nullptr) {
        fprintf(stderr, "%s",
            "The test data has been lost.Unable to "
            "continue testing.");
        exit(1);
    }
    return file;
}
