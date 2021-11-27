目录功能说明:

doc:帮助与说明文档
tests:测试用例，也是学习的好样例
bin:编译后的可执行文件
src:源文件:
	src/include:所有的头文件
	src/trc:主要的程序源文件
		src/Compiler:编译器
		src/Error:异常处理系统
		src/loader:转换器
		src/memory:内存管理
		src/share:多个模块共享的数据和函数
		src/tools:命令行工具
		src/TVM:执行字节码的虚拟机
		src/utils:开发过程中的工具函数存放模块
	src/language:各国语言的源文件
