/**
 * 由于c++本身并不支持大整数相加
 * 所以在这里通过trc_long来实现
 * 注意：该类重载了trc所支持的所有运算符
 * 包括+，-，*，/，==，！=等
 */

#pragma once

#include <string>
#include <ostream>
#include "base/trcdef.h"
#include "TVM/TVMdef.h"
#include "TVMbase/types/base.h"
#include "TVMbase/dll.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace types {
            class TRC_TVMbase_api trc_long : public trcobj {
            public:
                trc_long(const string &a);

                trc_long();

                def::OBJ operator=(def::OBJ a);

                def::OBJ operator+(def::OBJ a);

                def::OBJ operator-(def::OBJ a);

                def::OBJ operator*(def::OBJ v);

                def::OBJ operator/(def::OBJ);

                def::OBJ operator%(def::OBJ);

                def::OBJ pow(def::OBJ);

                def::OBJ zdiv(def::OBJ);

                def::INTOBJ operator!=(def::OBJ a);

                def::INTOBJ operator==(def::OBJ);

                def::INTOBJ operator<(def::OBJ);

                def::INTOBJ operator>(def::OBJ);

                def::INTOBJ operator<=(def::OBJ);

                def::INTOBJ operator>=(def::OBJ);

                void putline(ostream &out);

                const int &gettype();

                def::INTOBJ operator!();

                def::INTOBJ operator&&(def::OBJ value_i);

                def::INTOBJ operator||(def::OBJ value_i);

                ~trc_long();

                def::OBJ to_string();

                def::OBJ to_float();

                def::OBJ to_bool();

                void delete_();

            private:
                void set_alloc(int size_);

                const static int type = trc_long_T;

                // 第一位空出来，标识正负
                //之所以选择char，是因为每一位只需要保存一个数字，不需要int型
                char *value;

                // 当前大整数长度，注意，符号位也包括在内，正数有默认的符号位
                // 0可以被标识为正数，也可以被标识为负数，不受影响
                int size = 1;
            };
        }
    }
}
