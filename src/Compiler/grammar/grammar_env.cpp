#include "base/trcdef.h"
#include "grammar_env.h"
#include "Compiler/pri_Compiler.h"

using namespace std;

namespace trc
{
    namespace compiler
    {
        void grammar_data_control::array_get_value(treenode *head)
        {
            /**
             * 生成从数组中获取值的节点
             */
        }

        void grammar_data_control::create_array(treenode *head, const vecs &code)
        {
            /**
             * 生成创建数组的节点
             */
        }

        void grammar_data_control::map_get_value(treenode *head)
        {
            /**
             * 生成从map中获取值的节点
             */
        }

        void grammar_data_control::create_map(treenode *head, const vecs &code)
        {
            /**
             * 生成创建map的节点
             */
        }

        void grammar_data_control::compile_get_value(treenode *head, const vecs &code)
        {
            /**
             * 编译从map或者从array中取值的代码
             */
        }

        void grammar_data_control::compile_create_obj(treenode *head, const vecs &code)
        {
            /**
             * 编译从map或者从array中创建对象的代码
             */
        }

        grammar_data_control::~grammar_data_control()
        {
            /**
             * 清理数据
             */
            array_list.clear();
            map_list.clear();
        }

        struct node
        {
            token type;
            int data;
        };

        map<string, int> opti = {
            {"+", 1},
            {"-", 1},
            {"*", 2},
            {"/", 2},
            {"**", 3}};

        vecs change_to_last_expr(const vecs &line)
        {
            /**
             * 将一行中缀表达式转换成后缀表达式
             */
            vecs res;
            // vector<node*> st;
            // stack<char> oper_tmp;
            // for(int i = 0, n = line.length(); i < n; ++i) {
            //     if() {
            //         int tmp = 0;
            //         while('0' <= raw[i] && raw[i] <= '9') {
            //             tmp *= 10;
            //             tmp += raw[i] - '0';
            //             ++i;
            //         }
            //         st.push_back(new node{num, tmp});
            //         --i;
            //     } else {
            //         if(raw[i] == ')') {
            //             while(oper_tmp.top() != '(') {
            //                 st.push_back(new node{oper, oper_tmp.top()});
            //                 oper_tmp.pop();
            //             }
            //             oper_tmp.pop();
            //         } else {
            //             if(raw[i] != '(')
            //                 while(!oper_tmp.empty() && opti[oper_tmp.top()] != '(' && opti[raw[i]] <= opti[oper_tmp.top()]) {
            //                     st.push_back(new node{oper, oper_tmp.top()});
            //                     oper_tmp.pop();
            //                 }
            //             oper_tmp.push(raw[i]);
            //         }
            //     }
            // }
            // while(!oper_tmp.empty()) {
            //     st.push_back(new node{oper, oper_tmp.top()});
            //     oper_tmp.pop();
            // }
            return res;
        }
    }
}
