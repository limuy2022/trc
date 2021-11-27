/**
 * 关于文件系统和文件路径的操作
 */

#include <cstring>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <filesystem>
#include "base/Error.h"
#include "base/trcdef.h"
#include "base/utils/filesys.h"
#include "platform.h"
#include "base/dll.h"

namespace fs = std::filesystem;

using namespace std;

namespace trc {
    namespace utils {
        void listfiles(const string &dir, func_type func, vecs &fileList, vecs &dirList) {
            /**
             * 遍历目录
             * dir:遍历目录
             * func:判断文件是否应当存入的判别函数
             * fileList:遍历出文件的存储地
             * dirList：遍历出文件夹的存储地
             */

            string tmp;
            for(const auto& path_i : fs::recursive_directory_iterator(dir)) {
                if(is_directory(path_i)) {
                    fileList.push_back(path_i.path().string());
                    continue;
                }
                tmp = path_i.path().string();
                if(func(tmp)){
                    dirList.push_back(tmp);
                }
            }
        }

        bool check_file_is(const string &path) {
            /**
             * 检查文件是否存在
             */

            return fs::exists(path);
        }

        string path_last(const string &path, const string &last) {
            /**
             * 改变路径后缀名
             * path：路径
             * last：需要改变成的后缀名
             */
            return path.substr(0, path.find_last_of('.')) + last;
        }

        string import_to_path(string import_name) {
            /**
             * 在虚拟机执行过程中，动态加载字节码时路径转换
             * 例如：math.lang -> math/lang
             */

            size_t index;
            for (;;) {
                index = import_name.find('.');
                if (index != string::npos)
                    break;
                import_name[index] = '/';
            }
            return import_name;
        }

        string path_join(int n, ...) {
            /**
             * 路径粘贴
             * n：参数个数
             */

            va_list ap;
            va_start(ap, n);
            string root(va_arg(ap, const char*));
            for (int i = 1; i < n; ++i) {
                root += "\\";
                root += va_arg(ap, const char*);
            }
            va_end(ap);
            return root;
        }

        string file_last_(const string &path) {
            /**
             * 查找文件后缀名
             */
            return path.substr(path.find_last_of('.'));
        }

        void readcode(string &file_data, const string &path) {
            /*
            * 读取文件并返回字符串
            */

            file_data.clear();
            FILE *file = fopen(path.c_str(), "r");
            if (!file)
                error::send_error(error::OpenFileError, path.c_str());
            char tmp = fgetc(file);
            while (!feof(file)) {
                file_data += tmp;
                tmp = fgetc(file);
            }
            fclose(file);
        }

        void readfile(string &file_data, const string &path, error::error_type err, ...) {
            /**
             * 与上面那种方法不同在于可以自由选择报错类型，但失败同样会结束程序
             * err：错误名
             */

            file_data.clear();
            va_list ap;
            va_start(ap, err);
            FILE *file = fopen(path.c_str(), "r");
            if (!file) {
                error::send_error_(error::make_error_msg(err, ap));
                exit(1);
            }
            char tmp = fgetc(file);
            while (!feof(file)) {
                file_data += tmp;
                tmp = fgetc(file);
            }
            fclose(file);
            va_end(ap);
        }
    }
}
