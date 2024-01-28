# TVM指令集

| 指令集           |                作用 |
|:--------------|------------------:|
| ADD           |              两数相加 |
| SUB           |              两数相减 |
| MUL           |              两数相乘 |
| DIV           |              两数相除 |
| POW           |              两数乘方 |
| ZDIV          |                整除 |
| MOD           |               模运算 |
| NOP           |         这个指令什么也不做 |
| GOTO          |       跳转到指定的字节码索引 |
| STORE_NAME    |       从栈顶读取值并生成变量 |
| LOAD_NAME     |         取出变量的值并入栈 |
| DEL           |              删除变量 |
| CALL_BUILTIN  |            执行内置函数 |
| IF_FALSE_GOTO |        如果条件不成立则跳转 |
| LOAD_INT      |       加载int类型的值入栈 |
| LOAD_FLOAT    |     加载float类型的值入栈 |
| LOAD_STRING   |    加载string类型的值入栈 |
| CHANGE_VALUE  |            修改变量的值 |
| EQUAL         |         判断两个数是否相等 |
| UNEQUAL       |        判断两个数是否不相等 |
| GREATER       |     判断一个数是否大于另一个数 |
| LESS          |     判断一个数是否小于另一个数 |
| LESS_EQUAL    |   判断一个数是否小于等于另一个数 |
| GREATER_EQUAL |   判断一个数是否大于等于另一个数 |
| ASSERT        |           调试指令：断言 |
| NOT           | 取栈顶的值，取反后存入布尔值数据栈 |
| AND           |               与运算 |
| OR            |               或运算 |
| LOAD_ARRAY    |              创建数组 |
| CALL_METHOD   |              调用方法 |
