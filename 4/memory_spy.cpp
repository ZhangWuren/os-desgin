#include <Windows.h>
#include <iostream>
#include <Shlwapi.h>
#include <iomanip>
#pragma comment(lib, "shlwapi.lib")
using namespace std;

int main()
{
	MEMORYSTATUSEX statex;	//
	statex.dwLength = sizeof(statex);
	//获取系统内存信息
	GlobalMemoryStatusEx(&statex);
	printf("-----------------------内存信息-----------------------\n");
	//内存使用率
	printf("物理内存的使用率为:%ld%%\n", statex.dwMemoryLoad);
	//物理内存
	printf("物理内存的总容量为: %.2fGB.\n", (float)statex.ullTotalPhys / 1024 / 1024 / 1024);
	//可用物理内存
	printf("可用的物理内存为: %.2fGB.\n", (float)statex.ullAvailPhys / 1024 / 1024 / 1024);
	//提交的内存限制
	printf("总的交换文件为:%.2fGB.\n", (float)statex.ullTotalPageFile / 1024 / 1024 / 1024);
	//当前进程可以提交的最大内存量
	printf("可用的交换文件为：%.2fGB.\n", (float)statex.ullAvailPageFile / 1024 / 1024 / 1024);
	//虚拟内存
	printf("虚拟内存的总容量为：%.2fGB.\n", (float)statex.ullTotalVirtual/1024 / 1024 / 1024);
	//可用虚拟内存
	printf("可用的虚拟内存为：%.2fGB.\n", (float)statex.ullAvailVirtual/1024 / 1024 / 1024);
	//保留字段
	printf("保留字段的容量为：%.2fByte.\n",statex.ullAvailExtendedVirtual);
	
	return 0;
}