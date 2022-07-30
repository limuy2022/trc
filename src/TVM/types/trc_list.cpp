#include <TVM/TRE.h>
#include <TVM/types/trc_list.h>
#include <base/trcdef.h>
#include <cstdio>

namespace trc::TVM_space::types {
const RUN_TYPE_TICK trc_list::type = RUN_TYPE_TICK::array_T;

trc_list::trc_list() {
    head = new listnode;
    head->next = nullptr;
}

trc_list::~trc_list() {
    clear();
    delete head;
}

RUN_TYPE_TICK trc_list::gettype() {
    return type;
}

void trc_list::del(int index) {
    listnode* p = head;
    for (int i = 0; i < index - 1; ++i)
        p = p->next;
    p->next = p->next->next;
    delete p->next;
}

def::OBJ trc_list::operator[](int index) {
    listnode* p = head->next;
    for (int i = 0; i < index; ++i)
        p = p->next;
    return p->data;
}

void trc_list::insert(int index, def::OBJ data) {
    listnode *p = head, *s = new listnode, *temp;
    for (int i = 0; i < index; ++i)
        p = p->next;
    s->data = data;
    temp = p->next;
    p->next = s;
    s->next = temp;
}

void trc_list::append(def::OBJ data) {
    listnode *p = head, *s = new listnode;
    while (p->next != nullptr)
        p = p->next;
    p->next = s;
    s->data = data;
    s->next = nullptr;
}

int trc_list::len() {
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

void trc_list::clear() {
    listnode *t = head->next, *s;
    while (t != nullptr) {
        s = t;
        t = t->next;
        delete s;
    }
    head->next = nullptr;
}

void trc_list::putline(FILE* out) {
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

bool trc_list::check_in_(def::OBJ data) {
    listnode* t = head->next;
    while (t != nullptr) {
        if (TVM_share::obj_to_bool(data->operator==(t->data)))
            return true;
        t = t->next;
    }
    return false;
}
}
