#include <cmath>
#include "base/utils/type.hpp"
#include "TVM/TVM.h"
#include "TVMbase/TRE.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        void TVM::ADD() {
            /**
             * 支持拼接对象：int，float，string
             */
            secondv = pop();
            firstv = pop();
            push(firstv->operator+(secondv));
        }

        void TVM::SUB() {
            /**
             * 支持对象：int， float
             */
            secondv = pop();
            firstv = pop();
            push(firstv->operator-(secondv));
        }

        void TVM::MUL() {
            /**
             * 支持对象：int，float，string
             */
            secondv = pop();
            firstv = pop();
            push(firstv->operator*(secondv));
        }

        void TVM::DIV() {
            /**
             * 支持对象：int， float
             */
            secondv = pop();
            firstv = pop();
            push(firstv->operator/(secondv));
        }

        void TVM::ZDIV() {
            /**
             * 支持对象：int， float
             */
            secondv = pop();
            firstv = pop();
            push(firstv->zdiv(secondv));
        }

        void TVM::POW() {
            secondv = pop();
            firstv = pop();
            push(firstv->pow_(secondv));
        }

        void TVM::MOD() {
            secondv = pop();
            firstv = pop();
            push(firstv->operator%(secondv));
        }
    }
}
