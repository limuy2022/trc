module;
#include <cctype>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
export module compiler_def;
import TVM;
import Error;
import TVM_data;

export namespace trc::compiler {
/**
 * @brief 编译器的参数
 */
struct compiler_option {
    // 是否生成行号表
    bool number_table = true;
    // 是否开启优化
    bool optimize = false;
    // 是否进行常量折叠
    bool const_fold = true;
};

/**
 * @brief 判断是否为字母，包括_
 * @param c 字符
 */
inline bool is_english(char c) {
    return isalpha(c) || c == '_';
}

typedef size_t data_t;

/**
 * @brief 控制常量表的添加
 * @tparam T
 */
template <typename T> class constant_pool_controller {
private:
    std::unordered_map<T, size_t> index_saver;

public:
    std::vector<T>& ref;

    explicit constant_pool_controller(std::vector<T>& ref)
        : ref(ref) {
    }

    /**
     * @brief 添加常量
     * @return 常量地址
     */
    size_t add(T value) {
        size_t index = index_saver[value];
        if (index == 0 && (ref.empty() || ref.front() != value)) {
            index_saver[value] = ref.size();
            ref.push_back(value);
            return ref.size() - 1;
        }
        return index;
    }

    /**
     * @brief 获取常量的位置
     * @return std::numeric_limits<size_t>::max()代表不存在
     */
    size_t get(const T& value) {
        size_t index = index_saver[value];
        if (index == 0 && (ref.empty() || ref.front() != value)) {
            return std::numeric_limits<size_t>::max();
        }
        return index;
    }
};

/**
 * @brief 储存编译期间要用到的公共数据和成员
 */
class compiler_public_data {
public:
    compiler_public_data(const compiler_option& option, TVM_space::TVM_static_data& vm)
        : option(option)
        , vm(vm)
        , const_int(vm.const_i)
        , const_float(vm.const_f)
        , const_string(vm.const_s)
        , const_long_int(vm.const_long)
        , const_name(name_list) {
        // 添加true,false,null常量
        const_int.add(0);
        const_int.add(1);
    }
    const compiler_option& option;
    TVM_space::TVM_static_data& vm;
    constant_pool_controller<int> const_int;
    constant_pool_controller<double> const_float;
    std::vector<std::string> name_list;
    constant_pool_controller<std::string> const_string, const_long_int,
        const_name;

    /**
     * @brief 报出错误
     */
    template <typename... P>
    void send_error(error::error_type errorn, const P&... argv) {
        //todo:optimize it
        std::stringstream ss;
        error::send_error_interal<true>(errorn, ss.str(), argv...);
    }
};

// 实例化几个常用的编译参数
compiler_option optimize_option { true, true, true };
compiler_option nooptimize_option { true, false, true };
}
