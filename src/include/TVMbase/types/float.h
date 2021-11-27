#pragma once

#include "base/trcdef.h"
#include "TVM/TVMdef.h"
#include "TVMbase/types/base.h"
#include "TVMbase/dll.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace types {
            class TRC_TVMbase_api trc_float : public trcobj {
            public:
                double value;

                void putline(ostream &out);

                ~trc_float();

                trc_float();

                trc_float(const double &value);

                def::OBJ to_int();

                def::OBJ to_string();

                def::OBJ to_bool();

                def::INTOBJ operator==(def::OBJ value_i);

                def::INTOBJ operator!=(def::OBJ value_i);

                def::INTOBJ operator<(def::OBJ value_i);

                def::INTOBJ operator>(def::OBJ value_i);

                def::INTOBJ operator<=(def::OBJ value_i);

                def::INTOBJ operator>=(def::OBJ value_i);

                def::OBJ operator+(def::OBJ value_i);

                def::OBJ operator-(def::OBJ value_i);

                def::OBJ operator*(def::OBJ value_i);

                def::OBJ operator/(def::OBJ value_i);

                def::OBJ operator%(def::OBJ value_i);

                def::OBJ pow_(def::OBJ value_i);

                def::OBJ zdiv(def::OBJ value_i);

                def::INTOBJ operator!();

                def::INTOBJ operator&&(def::OBJ value_i);

                def::INTOBJ operator||(def::OBJ value_i);

                const int &gettype();

            private:
                // 整型标记类型
                const static int type = float_T;
            };
        }
    }
}
