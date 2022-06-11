#include <test_new_del.h>

namespace count_new_del {
int del_cnt = 0, new_cnt = 0;

test_new_del::test_new_del() {
    new_cnt++;
}

test_new_del::~test_new_del() {
    del_cnt++;
}

void reset() {
    del_cnt = 0;
    new_cnt = 0;
}
}
