module;
#include <cstdio>
export module trc_list;
import TVMdef;
import trcdef;
import base;
import TRE;

namespace trc::TVM_space::types {
/**
 * trc中的链表结构
 */
class trc_list : public trcobj {
    struct listnode {
        def::OBJ data;
        listnode* next;
    };

public:
    // 头尾结点，头结点不存数据
    listnode *head, *tail = nullptr;

    trc_list();

    ~trc_list() override;

    /**
     * @brief 删除链表中index的元素
     */
    void del(int index) const;

    /**
     * @brief 获取index的值
     */
    def::OBJ operator[](int index) const;

    /**
     * @brief 在index前插入data
     */
    void insert(int index, def::OBJ data) const;

    /**
     * @brief 在链表末尾添加data
     */
    void append(def::OBJ data) const;

    /**
     * @brief 计算链表长度,不算头结点
     */
    size_t len() const;

    /**
     * @brief 清空链表
     * @warning 不会删除头结点
     */
    void clear() const;

    /*
     * @brief 以[0,9,10]的方式打印整个链表
     */
    void putline(FILE* out) override;

    /**
     * @brief 检查元素是否在链表中
     * @warning 检查值，不检查地址
     */
    bool check_in_(def::OBJ data) const;

    RUN_TYPE_TICK gettype() override;

private:
    static const RUN_TYPE_TICK type;
};

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

void trc_list::del(int index) const {
    listnode* p = head;
    for (int i = 0; i < index - 1; ++i)
        p = p->next;
    p->next = p->next->next;
    delete p->next;
}

def::OBJ trc_list::operator[](int index) const {
    listnode* p = head->next;
    for (int i = 0; i < index; ++i)
        p = p->next;
    return p->data;
}

void trc_list::insert(int index, def::OBJ data) const {
    listnode *p = head, *s = new listnode, *temp;
    for (int i = 0; i < index; ++i)
        p = p->next;
    s->data = data;
    temp = p->next;
    p->next = s;
    s->next = temp;
}

void trc_list::append(def::OBJ data) const {
    listnode *p = head, *s = new listnode;
    while (p->next != nullptr)
        p = p->next;
    p->next = s;
    s->data = data;
    s->next = nullptr;
}

size_t trc_list::len() const {
    listnode* t = head->next;
    if (head->next == nullptr) {
        // 为空
        return 0;
    }
    size_t length = 1;
    while (t->next != nullptr) {
        length++;
        t = t->next;
    }
    return length;
}

void trc_list::clear() const {
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

bool trc_list::check_in_(def::OBJ data) const {
    listnode* t = head->next;
    while (t != nullptr) {
        if (TVM_share::obj_to_bool(data->operator==(t->data)))
            return true;
        t = t->next;
    }
    return false;
}
}
