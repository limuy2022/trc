#include "TVM/TRE.h"
#include "TVM/types/array.h"
#include "base/trcdef.h"
#include <cstdio>

namespace trc::TVM_space::types {
const RUN_TYPE_TICK trc_array::type = RUN_TYPE_TICK::array_T;

trc_array::trc_array() {
    head = new listnode;
    head->next = nullptr;
}

trc_array::~trc_array() {
    clear();
    delete head;
}

RUN_TYPE_TICK trc_array::gettype() {
    return type;
}

void trc_array::del(int index) {
    listnode* p = head;
    for (int i = 0; i < index - 1; ++i)
        p = p->next;
    p->next = p->next->next;
    delete p->next;
}

def::OBJ trc_array::operator[](int index) {
    listnode* p = head->next;
    for (int i = 0; i < index; ++i)
        p = p->next;
    return p->data;
}

void trc_array::insert(int index, def::OBJ data) {
    listnode *p = head, *s = new listnode, *temp;
    for (int i = 0; i < index; ++i)
        p = p->next;
    s->data = data;
    temp = p->next;
    p->next = s;
    s->next = temp;
}

void trc_array::append(def::OBJ data) {
    listnode *p = head, *s = new listnode;
    while (p->next != nullptr)
        p = p->next;
    p->next = s;
    s->data = data;
    s->next = nullptr;
}

int trc_array::len() {
    listnode* t = head->next;
    if (head->next == nullptr)
        return 0; // 为空

    int length = 1;
    while (t->next != nullptr) {
        length++;
        t = t->next;
    }
    return length;
}

void trc_array::clear() {
    listnode *t = head->next, *s;
    while (t != nullptr) {
        s = t;
        t = t->next;
        delete s;
    }
    head->next = nullptr;
}

void trc_array::putline(FILE* out) {
    listnode* t = head->next;
    fputc('[', out);
    while (t->next != nullptr) {
        t->data->putline(out);
        fputc(',', out);
        t = t->next;
    }
    t->data->putline(out);
    fputc(']', out);
}

bool trc_array::check_in_(def::OBJ data) {
    listnode* t = head->next;
    while (t != nullptr) {
        if (TVM_share::obj_to_bool(data->operator==(t->data)))
            return true;
        t = t->next;
    }
    return false;
}
}
