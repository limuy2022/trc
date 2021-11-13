/**
 * 这个源文件并不会被可执行文件所链接
 * 它是为了方便编译操作存在 
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

bool build_vs() {
    if (system("cmake .")) {
        return 0;
    }
    system("trc.sln");
    return 1;
}

bool build_gcc() {
    if (system("cmake . -G \"MinGW Makefiles\"") || system("make")) {
        return 0;
    }
    system("strip bin/trc.exe");
    return 1;
}

bool build_msvc() {
    if (system("cmake . -G \"NMake Makefiles\"") || system("nmake")) {
        return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("error:please read the readme.md to learn how to use buildtrc to compile this project.");
        exit(0);
    }
    const char *str = argv[1];
    if (!strcmp(str, "vs")) {
        if (!build_vs()) {
            printf("error:can\'t use vs to compile this project.");
            exit(0);
        }
    } else if (!strcmp(str, "gcc")) {
        if (!build_gcc()) {
            printf("error:can\'t use gcc to compile this project.");
            exit(0);
        }
    } else if (!strcmp(str, "msvc")) {
        if (!build_msvc()) {
            printf("error:can\'t use msvc to compile this project.");
            exit(0);
        }
    }
    printf("built.");
}
	