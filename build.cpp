/**
 * 这个源文件并不会被可执行文件所链接
 * 它是为了方便编译操作存在 
 */

#include <cstdlib>
#include <cstdio>

using namespace std;

inline bool build(){
	if (!system("make -j8")){
		system("strip bin/trc.exe");
		system("strip bin/libTVM.dll");
		system("strip bin/libCompiler.dll");
		system("strip bin/libshare.dll");
		return true;
	}
	return false;
}
	
int main(){
	if(!build()){
		system("cmake . -G \"MinGW Makefiles\"");
		build();
	}
	printf("built.");
	getchar();
}
	