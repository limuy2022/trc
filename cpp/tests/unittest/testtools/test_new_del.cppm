export module test_new_del;

export namespace count_new_del {

/**
 * @brief 记录构造函数和析构函数被调用次数
 * 用于检测内存是否释放，函数是否被调用等
 */
class test_new_del {
public:
    test_new_del();

    ~test_new_del();
};

int del_cnt = 0;
int new_cnt = 0;

test_new_del::test_new_del() {
    new_cnt++;
}

test_new_del::~test_new_del() {
    del_cnt++;
}

/**
 * @brief 重新初始化构造值和析构值
 * @warning 必须在每个新的测试开头执行
 */
void reset() {
    del_cnt = 0;
    new_cnt = 0;
}
}
