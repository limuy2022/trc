#ifndef TRC_INCLUDE_TVM_BASIC_VM_H
#define TRC_INCLUDE_TVM_BASIC_VM_H

#include <map>
#include <string>
#include <stack>
#include "../cfg.h"

using namespace std;

class frame_;

class base {
	/**
	 * 虚拟机结构的基本
	 * 派生类：例如：TVM， frame_
	 * TVM运行过程中数据的存放地
	 */ 
public:
	// 变量
	map<string, OBJ> var_names;
	// 操作数栈
	stack<OBJ> stack_data;
	// 帧栈
    stack<frame_ *> frames;
};

#endif
