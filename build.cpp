/**
 * 这个源文件并不会被可执行文件所链接
 * 它是为了方便编译操作存在 
 */
/**
 * 不要真正把它应用于项目之中，它除了编译一无是处
 */ 

#include <cstdlib>
#include <cstdio>

using namespace std;

bool build(){
	if (!system("make -j8")){
		system("strip bin/trc.exe");
		system("strip bin/libTVM.dll");
		system("strip bin/libCompiler.dll");
		system("strip bin/libshare.dll");
		system("strip bin/libimportlib.dll");
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
	