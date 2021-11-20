/**
 * 该头文件是私有头文件，不是公有头文件
 */ 

#ifndef TRC_SRC_COMPILER_GRAMMAR_OBJ_H
#define TRC_SRC_COMPILER_GRAMMAR_OBJ_H

#include "trcdef.h"

using namespace std;

extern vecs array_list;
extern vecs map_list;

namespace map_arr {
    void compile_get_value(treenode *head, const vecs &code);

    void compile_create_obj(treenode *head, const vecs &code);

    inline void clear() {
        /**
         * 清理数据
         */
        array_list.clear();
        map_list.clear();
    }
}

#endif
