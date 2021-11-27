#pragma once

using namespace std;

namespace trc {
    namespace loader {
        TRC_base_api void loader_ctree(TVM_space::TVM *vm, const string &path);

        TRC_base_api void save_ctree(TVM_space::TVM *vm, const string &path);

        TRC_base_api bool is_magic(const string &path);
    }
}
