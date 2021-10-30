/**
 * 与TVM库文件有关的定义
 */ 

#ifndef TRC_INCLUDE_TVM_LIB_H
#define TRC_INCLUDE_TVM_LIB_H

#include <map>
#include "cfg.h"

using namespace std;

class cpp_lib {
	/**
	 * c++编写的标准库的信息描述，便于进行调用
	 */ 

public:
	// 函数名的定义
	char ** funcs;

	int funcs_nums;

	map<string, OBJ> vars; 

	cpp_lib(int);

	~cpp_lib();
};

namespace cpp_libs {
	/**
	 * 关于外部调用库
	 */ 
	extern map<string, cpp_lib> load_cpp_libs;
	extern size_t libs_num;
	extern string names[];
}

#endif
