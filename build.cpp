/**
 * 这个源文件并不会被可执行文件所链接
 * 它是为了方便编译操作存在 
 */

#include <cstdlib>
#include <cstdio>

using namespace std;

bool build(){
	if (!system("make -j8")){
		system("strip bin/trc.exe");
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
	