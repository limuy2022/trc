#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" EXPORT int add(int a, int b) {
    return a + b;
}

extern "C" EXPORT const char* return_(const char* str) {
    return str;
}
