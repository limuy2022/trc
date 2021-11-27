#pragma once

#include <string>
#include "dll.h"

using namespace std;

namespace trc {
    namespace error {
        namespace error_env {
            /**
             * 报错系统需要知道当前处于什么模式，合理应对发生的状况
             */
            TRC_base_api extern bool quit;
            TRC_base_api extern bool out_msg;
        }

        // 错误，增强可读性
        enum error_type {
            NameError,
            ValueError,
            TypeError,
            SyntaxError,
            VersionError,
            OpenFileError,
            ModuleNotFoundError,
            ArgumentError,
            ZeroDivError,
            RunError,
            AssertError,
            IndexError,
            MemoryError,
            KeyError,
        };

        TRC_base_api void send_error(error_type name, ...);

        TRC_base_api string make_error_msg(error_type error_name, va_list &ap);

        TRC_base_api void send_error_(const string &error_msg) noexcept;
    }
}
