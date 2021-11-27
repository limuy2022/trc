/**
 * 高精度浮点数运算
 */

#include <string>
#include "TVMbase/types/flong.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace types {
            const int trc_flong::type;

            trc_flong::trc_flong(const string &) {

            }

            trc_flong::trc_flong(double init_data) {

            }

            trc_flong::trc_flong() {

            }

            trc_flong::~trc_flong() {

            }

            void trc_flong::putline(ostream &out) {

            }

            const int &trc_flong::gettype() {
                return type;
            }

            def::INTOBJ trc_flong::operator==(def::OBJ value_i) {
                return def::INTOBJ(nullptr);
            }

            def::INTOBJ trc_flong::operator!=(def::OBJ value_i) {
                return def::INTOBJ(nullptr);
            }

            void trc_flong::set_realloc(size_t num) {

            }
        }
    }
}
