/**
 * 声明trc定义的所有外调tools函数
 * tools格式：由一个供调用的函数以及一个函数依赖的基本实现
 * 例如：clean,__clean
 */

#pragma once

using namespace std;

namespace trc {
    namespace TVM_space {
        class TVM;
    }
    namespace tools_in {
        /**
         * 工具内部实现
         */
    
        void __brun(TVM_space::TVM *vm, const string &path);
    
        void __build(TVM_space::TVM *vm, const string &path);
    
        void __dis(TVM_space::TVM *vm, const string &file_path);
    
        void __out_grammar(const string &path);
    
        void __run(TVM_space::TVM *vm, const string &path);
    
        void __out_token(const string &path);
    }
}

namespace trc {
    namespace tools_out {
        /**
         * 工具外部接口
         */
    
        void brun(int argc, char *argv[]);
    
        void build(int argc, char *argv[]);
    
        void dis(int argc, char *argv[]);
    
        void out_grammar(int argc, char *argv[]);
    
        void run(int argc, char *argv[]);
    
        void out_token(int argc, char *argv[]);
    
        void tshell();
    
        void tdb();

        void help();
    }
}
