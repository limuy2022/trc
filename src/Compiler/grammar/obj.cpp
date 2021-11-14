/**
 * 关于对象的代码转换为节点以及对象信息保存和识别
 * 其实就相当于编译时的解释器，负责解析代码并把变量的作用域和类型做出标识，用于各种判断
 */

#include "cfg.h"
#include "node.h"

using namespace std;

// 由于map和array获取值的方式相同但是生成的字节码不同，需要在定义时予以区分
vecs array_list;
vecs map_list;

static void array_get_value(treenode *head) {
    /**
     * 生成从数组中获取值的节点
     */
}

static void create_array(treenode *head, const vecs &code) {
    /**
     * 生成创建数组的节点
     */
}

static void map_get_value(treenode *head) {
    /**
     * 生成从map中获取值的节点
     */
}

static void create_map(treenode *head, const vecs &code) {
    /**
     * 生成创建map的节点
     */
}

namespace map_arr {
    void compile_get_value(treenode *head, const vecs &code) {
        /**
         * 编译从map或者从array中取值的代码
         */
    }

    void compile_create_obj(treenode *head, const vecs &code) {
        /**
         * 编译从map或者从array中创建对象的代码
         */
    }
}
