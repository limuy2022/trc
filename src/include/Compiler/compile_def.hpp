#pragma once

#include <cctype>
#include <array>
#include <string>
#include "base/trcdef.h"
#include "base/utils/data.hpp"
#include "dll.h"

using namespace std;

// 根据返回值判断解析是否完成
#define END_OF_LEX "\0"

namespace trc {
	namespace compiler {
		extern array<char, 12> num;
		extern array<string, 4> sentences;
		extern array<string, 1> sentences_yes_argv;
		extern array<string, 5> keywords;
		extern array<string, 7> opti_opers;
		extern array<string, 8> opti_condits;
		extern array<string, 9> condits;
		extern array<string, 2> highest_condits;
		extern array<string, 6> other_condits;
		extern array<string, 2> aslist;
		extern array<string, 14> opers;
		extern array<string, 15> in_func_char;
		extern array<string, 3> const_values;
		extern map<string, int> change_const;
		extern map<char, string> change_varchar;

		// 编译时类型标识
		enum COMPILE_TYPE_TICK {
		    VAR_TICK=1,
		    int_TICK,
		    float_TICK,
		    string_TICK,
		    LONG_TICK,
		    FLOAT_L_TICK,
		    CONST_TICK,
		};

		// 语法生成树中的标识
		enum grammar_type {
			// 纯粹数据
			DATA,
			// 生成无参数字节码
			OPCODE,
			// 生成无法通过符号转化的字节码
			ORIGIN,
			// 内置函数
			BUILTIN_FUNC,
			// 树
			TREE,
			// 带有参数的字节码
			OPCODE_ARGV,
			// 函数定义，由于字节码十分特殊
			FUNC_DEFINE,
			// 变量定义，由于变量作用域特殊
			VAR_DEFINE,
			// 调用自定义函数
			CALL_FUNC,
		};

		class treenode {
		    /**
		     * 树节点，生成语法分析树时用到
		     */
		public:
		    grammar_type type;

		    treenode *father = nullptr;
            
            vector<treenode *> son;
		    
		    char* data;

		    void connect(treenode *son_connect);

		    treenode(grammar_type type_argv, const string& data);

		    treenode(grammar_type type = TREE);

		    ~treenode();

			void set_alloc(size_t sizes);
		private:
			bool is_alloc = false;
		};

		typedef string token_type;

		enum class blocks_type {
			IF_TYPE,
			WHILE_TYPE,
			FUNC_TYPE
		};

		inline bool is_num(char c) {
			/* 宏，判断是否为数字，包括-和. */
			return utils::check_in(c, num);
		}
		//
		inline bool is_english(char c) {
			/* 判断是否为字母，包括_ */
			return isalpha(c) || c == '_';
		}
		// 判断是否为数字或字符，不包含.和-，包含_
		inline bool is_english_num(char c) {
			return isdigit(c) || is_english(c);
		}

		TRC_Compiler_api COMPILE_TYPE_TICK what_type(const string &value);
	}
}
