/**
 * 预处理器
 * 转化格式并删去不必要字符
 */

#include <string>
#include "share.h"

using namespace std;

void pre(vecs &result, const string &codes) {
    /**
    * 代码简单初始化，去掉多余字符并且转化格式
    * codes：源代码
    */

    string temp;
    for (const auto &code: codes) {
        if (code == '\n') {
            if (temp[0] != '#') {
                result.push_back(temp);
            }
            temp.clear();
        } else if (code != '\t' && code != ' ') {
            temp += code;
        }
    }
    result.push_back(temp);
}

void pre_token_2(vecs2d &tokens) {
    /**
     * 代码二次更正，此次更正在执行token分词之后，进行代码检查之前，目的在于矫正tokens
     * 防止grammar出现错误的结果
     */

    for (auto &i: tokens)
        for (int j = 0, n = i.size(); j < n; ++j)
            if (i[j] == "#")
                for (; j < n; n--)
                    i.pop_back(); // 去除注释
}
