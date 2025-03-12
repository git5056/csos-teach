#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <string>
// using namespace std;

inline UINT64 GetCount()
{
	// __asm__ volatile (
	// 	"rdtsc\n");
    UINT64 low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return (UINT64)high << 32 | low;

}

// 获取CPU时钟频率
int GetCPUTime()
{
	std::string strCPUTime;
 
	UINT64 start, end;
	LONG ticks=0, ticksextra=0;
 
	printf("%ld.%ldMHZ\n", ticks, ticksextra);
	start = GetCount();
	end = GetCount() - start;
	start = GetCount();
 
	printf("%ld.%ldMHZ\n", ticks, ticksextra);
	Sleep(1000);
	unsigned cpuSpeed = (unsigned)( (GetCount() - start - end) / 1000 );
	ticks = cpuSpeed / 1000;
	ticksextra = cpuSpeed - (ticks * 100);
 
	printf("%ld.%ldMHZ\n", ticks, ticksextra);
	// strCPUTime.fo("%ld.%ldMHZ", ticks, ticksextra);
	return 0;
}

int main(){
	GetCPUTime();
	std::printf("aa\n");
	std::string aa="aa";
	aa.append("azxc");
	std::printf("aa:%s\n",aa.c_str());
	std::cout<<"aaa"<<std::endl;
	return 0;
}