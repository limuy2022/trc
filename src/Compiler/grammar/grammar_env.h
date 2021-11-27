/**
 * 该头文件是私有头文件，不是公有头文件
 */

#pragma once

#include "base/trcdef.h"

using namespace std;

namespace trc
{
    namespace compiler
    {
        class treenode;
        class grammar_data_control
        {
            /**
             * 关于对象的代码转换为节点以及对象信息保存和识别
             * 其实就相当于编译时的解释器，负责解析代码并把变量的作用域和类型做出标识，用于各种判断
             */
        public:
            void compile_get_value(treenode *head, const vecs &code);

            void compile_create_obj(treenode *head, const vecs &code);

            ~grammar_data_control();

        private:
            vecs array_list;

            vecs map_list;

            // 由于map和array获取值的方式相同但是生成的字节码不同，需要在定义时予以区分
            void array_get_value(treenode *head);

            void create_array(treenode *head, const vecs &code);

            void map_get_value(treenode *head);

            void create_map(treenode *head, const vecs &code);
        };

        vecs change_to_last_expr(const vecs &);
    }
}
