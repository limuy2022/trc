/**
 * 声明trc定义的所有外调tools函数
 * tools格式：由一个供调用的函数以及一个函数依赖的基本实现
 * 例如：clean,__clean
 */

#ifndef TRC_INCLUDE_TOOLS_H
#define TRC_INCLUDE_TOOLS_H

using namespace std;

class TVM;

namespace tools_in {
    /**
     * 工具内部实现
     */

    void __brun(TVM *vm, const string &path);

    void __build(TVM *vm, const string &path);

    void __all(TVM *vm, const string &path);

    void __clean(const string &path);

    void __crun(TVM *vm_run, const string &path);

    void __dis(TVM *vm, const string &file_path);

    void __out_grammar(const string &path);

    void __run(TVM *vm, const string &path);

    void __out_token(const string &path);
}

namespace tools_out {
    /**
     * 工具外部接口
     */

    void brun(int argc, char *argv[]);

    void build(int argc, char *argv[]);

    void all(int argc, char *argv[]);

    void clean(int argc, char *argv[]);

    void crun(int, char *argv[]);

    void dis(int argc, char *argv[]);

    void out_grammar(int argc, char *argv[]);

    void run(int argc, char *argv[]);

    void out_token(int argc, char *argv[]);

    void tshell();

    void tdb();
}

#endif
