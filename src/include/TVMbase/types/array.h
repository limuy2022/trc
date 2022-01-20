#pragma once

#include "TVM/TVMdef.h"
#include "TVMbase/dll.h"
#include "TVMbase/types/base.h"
#include "base/trcdef.h"

namespace trc::TVM_space::types {
/**
 * trc中的链表结构
 */
class TRC_TVMbase_api trc_array : public trcobj {
    struct listnode {
        def::OBJ data;
        listnode* next;
    };

public:
    // 头尾结点，头结点不存数据
    listnode *head, *tail = nullptr;

    trc_array();

    ~trc_array();

    /**
     * @brief 删除链表中index的元素
     */
    void del(int index);

    /**
     * @brief 获取index的值
     */
    def::OBJ operator[](int index);

    /**
     * @brief 在index前插入data
     */
    void insert(int index, def::OBJ data);

    /**
     * @brief 在链表末尾添加data
     */
    void append(def::OBJ data);

    /**
     * @brief 计算链表长度,不算头结点
     */
    int len();

    /**
     * @brief 清空链表
     * @warning 不会删除头结点
     */
    void clear();

    /*
     * @brief 以[0,9,10]的方式打印整个链表
     */
    void putline(FILE* out);

    /**
     * @brief 检查元素是否在链表中
     * @warning 检查值，不检查地址
     */
    bool check_in_(def::OBJ data);

    RUN_TYPE_TICK gettype();

private:
    static const RUN_TYPE_TICK type;
};
}
