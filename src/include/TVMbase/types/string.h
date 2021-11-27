#pragma once

#include <string>
#include "base/trcdef.h"
#include "TVM/TVMdef.h"
#include "TVMbase/types/base.h"
#include "TVMbase/dll.h"

using namespace std;

namespace trc
{
    namespace TVM_space
    {
        namespace types
        {
            class TRC_TVMbase_api trc_string : public trcobj
            {
            public:
                // 字符数(不包括\0)
                size_t char_num = 0;

                char *value;

                trc_string(const trc_string &init);

                trc_string &operator=(const string &);

                trc_string(const string &init);

                trc_string();

                ~trc_string();

                size_t len();

                char &operator[](unsigned int index);

                const char &operator[](unsigned int index) const;

                trc_string &operator=(const trc_string &value_i);

                def::OBJ operator+=(def::OBJ value_i);

                const char *c_str();

                void putline(ostream &out);

                void in(istream &in_);

                def::OBJ to_int();

                def::OBJ to_float();

                def::INTOBJ operator==(def::OBJ value_i);

                def::INTOBJ operator!=(def::OBJ value_i);

                def::INTOBJ operator<(def::OBJ value_i);

                def::INTOBJ operator>(def::OBJ value_i);

                def::INTOBJ operator<=(def::OBJ value_i);

                def::INTOBJ operator>=(def::OBJ value_i);

                def::OBJ operator+(def::OBJ value_i);

                def::OBJ operator*(def::OBJ value_i);

                const int &gettype();

                void delete_();

            private:
                void set_realloc(size_t num);

                // 整型标记类型
                const static int type = string_T;
            };
        }
    }
}
