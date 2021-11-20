/**
 * 关于文件系统和文件路径的操作
 */

#include <cstring>
#include <string>
#include <cstdarg>
#include <fstream>
#include "Error.h"
#include "trcdef.h"
#include "platform.h"
#ifdef WINDOWS_PLAT
    #include <io.h>
#elif defined(LINUX_PLAT)
    #include <dirent.h>
#endif

using namespace std;

void listfiles(const string &dir, const string &file, vecs &fileList, vecs &dirList) {
    /**
     * 遍历目录
     * dir:遍历目录
     * file:需要遍历的文件
     * fileList:遍历出文件的存储地
     * dirList：遍历出列表的存储地
     */

    #ifdef LINUX_PLAT
    DIR * dp = opendir(dir.c_str());
    dirent * dirp;
    if(dp == nullptr) {
        send_error(OpenFileError, dir.c_str());
    }
    while((dirp = readdir(dp)) != nullptr) {
        if(dirp -> d_type & DT_REG) {
            fileList.push_back(dirp->d_name);
        } else if(dirp -> d_type & DT_DIR) {
            dirList.push_back(dirp->d_name);
        }
    }
    closedir(dp);
    #elif defined(WINDOWS_PLAT)
    /* 在目录后面加上file进行第一次搜索. */
    const string &dirNew(dir + file);

    intptr_t handle;
    /* _finddata_t是存储文件各种信息的结构体 */
    _finddata_t findData;

    handle = _findfirst(dirNew.c_str(), &findData);
    /* 检查是否成功 */
    if (handle == -1) return;
    do {
        if (findData.attrib & _A_SUBDIR) {
            // 目录
            if (!strcmp(findData.name, ".") || !strcmp(findData.name, "..")) continue;
            dirList.push_back(findData.name);
        } else
            fileList.push_back(findData.name);  // 文件

    } while (!_findnext(handle, &findData));
    _findclose(handle);    /* 关闭搜索句柄 */
    #endif
}

bool check_file_is(const string &path) {
    /**
     * 检查文件是否存在
     */

    return !access(path.c_str(), 0);
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
        index = import_name.find(".");
        if (index != string::npos)
            import_name.replace(index, 1, "/");
        else
            break;
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
    string root(va_arg(ap, const string));
    for (int i = 1; i < n; ++i) {
        root += "\\";
        root += va_arg(ap, const string);
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
