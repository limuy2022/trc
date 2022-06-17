#include <TVMbase/TVM_data.h>

namespace trc::TVM_space {
symbol_form::~symbol_form() {
    delete[] vars;
}

void TVM_static_data::ReleaseStringData() {
    for (auto i = const_s.begin() + 1, end = const_s.end(); i != end; ++i) {
        delete[] * i;
    }
    const_s.clear();
    const_s.push_back(nullptr);
    for (auto i = const_name.begin() + 1, end = const_name.end(); i != end;
         ++i) {
        delete[] * i;
    }
    const_name.clear();
    const_name.push_back(nullptr);
    for (auto i = const_long.begin() + 1, end = const_long.end(); i != end;
         ++i) {
        delete[] * i;
    }
    const_long.clear();
    const_long.push_back(nullptr);
}

TVM_static_data::~TVM_static_data() {
    ReleaseStringData();
}

// 初始化为1个是因为索引为uint16_t，所以0就表示无参数，0号位必须放一个数占位
TVM_static_data::TVM_static_data()
    : const_i(1)
    , const_s(1)
    , const_name(1)
    , const_long(1)
    , const_f(1) {
}
}
