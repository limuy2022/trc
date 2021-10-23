#ifndef TRC_TVM_ARRAY_H
#define TRC_TVM_ARRAY_H

#include "base.h"
#include "../cfg.h"

using namespace std;

class trc_array : public trcobj
{
    /*
    链表
    */

    struct listnode {
        OBJ data;
        listnode *next;
    };

public:
    listnode *head, *tail = nullptr;

    trc_array();

    ~trc_array();

    void del(int index);

    OBJ operator [](int index);

    void insert(int index, OBJ data);

    void append(OBJ data);

    int len();

    void clear();

    void print();

    bool check_in(OBJ data);
};

#endif
