#pragma once

#include "base/trcdef.h"
#include "base/Error.h"


using namespace std;

namespace trc {
    namespace utils {
        typedef bool (*func_type)(const string &);

        TRC_base_api void listfiles(const string &dir, const string &file, vecs &fileList, vecs &dirList);

        TRC_base_api bool check_file_is(const string &path);

        TRC_base_api string path_last(const string &path, const string &last);

        TRC_base_api string import_to_path(string import_name);

        TRC_base_api string path_join(int n, ...);

        TRC_base_api string file_last_(const string &path);

        TRC_base_api void readcode(std::string &file_data, const string &path);

        TRC_base_api void readfile(std::string &file_data, const string &path, error::error_type err, ...);
    }
}
