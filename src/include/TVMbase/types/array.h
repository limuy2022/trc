#pragma once

#include "TVMbase/types/base.h"
#include "base/trcdef.h"
#include "TVMbase/dll.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace types {
            class TRC_TVMbase_api trc_array : public trcobj {
                /*
                链表
                */
            
                struct listnode {
                    def::OBJ data;
                    listnode *next;
                };
            
            public:
                listnode *head, *tail = nullptr;
            
                trc_array();
            
                ~trc_array();
            
                void del(int index);
            
                def::OBJ operator[](int index);
            
                void insert(int index, def::OBJ data);
            
                void append(def::OBJ data);
            
                int len();
            
                void clear();
            
                void print();
            
                bool check_in_(def::OBJ data);
            };
        }
    }
}
