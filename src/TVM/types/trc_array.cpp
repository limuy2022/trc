#include <iostream>
#include "cfg.h"
#include "TVM/array.h"

using namespace std;

trc_array::trc_array() {
    head = new listnode;
    head->next = nullptr;
}

trc_array::~trc_array() {
    clear();
    delete head;
};

void trc_array::del(int index) {
    /*
    *删除链表中index
    */
    listnode *p = head;
    for (int i = 0; i < index - 1; ++i)
        p = p->next;
    p->next = p->next->next;
    delete p->next;
}

OBJ trc_array::operator [](int index) {
    /*
    * 获取index的值
    */

    listnode *p = head -> next;
    for (int i = 0; i < index; ++i)
        p = p->next;
    return p->data;
}

void trc_array::insert(int index, OBJ data) {
    /*
    * 在index前插入data
    */
    listnode *p = head, *s = new listnode, *temp;
    for (int i = 0; i < index; ++i)
        p = p->next;
    s->data = data;
    temp = p->next;
    p->next = s;
    s->next = temp;
}

void trc_array::append(OBJ data) {
    /*
    *在链表末尾添加data
    */
    listnode *p = head, *s = new listnode;
    while (p->next != nullptr)
        p = p->next;
    p->next = s;
    s->data = data;
    s->next = nullptr;
}

int trc_array::len() {
    /*
    * 计算链表长度,不算头结点
    */
    listnode *t = head->next;
    if (head->next == nullptr)  return 0;    // 为空

    int length = 1;
    while (t->next != nullptr) {
        length++;
        t = t->next;
    }
    return length;
}

void trc_array::clear() {
    /*
    * 清空链表
    */
    listnode *t = head->next, *s;
    while (t != nullptr) {
        s = t;
        t = t->next;
        delete s;
    }
    head->next = nullptr;
}

void trc_array::print() {
    /*
    *打印整个链表
    */
    listnode *t = head -> next;
    if (head->next == nullptr) {
        // 为空
        cout << "[]";
        return;
    }
    cout << "[";
    while (t->next != nullptr) {
        cout << t->data << ',';
        t = t->next;
    }
    cout << t -> data << "]";
}

bool trc_array::check_in(OBJ data) {
    listnode *t = head->next;
    while (t != nullptr) {
        if (data == t->data) return true;
        t = t->next;
    }
    return false;
}
