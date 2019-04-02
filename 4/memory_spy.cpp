#include <Windows.h>
#include <iostream>
#include <Shlwapi.h>
#include <iomanip>
#define DIV 1024
#pragma comment(lib, "shlwapi.lib")
using namespace std;

int main()
{
	//来自https://docs.microsoft.com/zh-cn/windows/desktop/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	//获取物理内存信息
	if (GlobalMemoryStatusEx(&statex) == false)
	{
		cout << GetLastError() << endl;
	}
	cout << "-------------------statement of memory-------------------" << endl;
	cout << "The statement of physical memory: " << endl;
	//内存使用
	cout << "There is " << statex.dwLength << " percent of memory in use." << endl;
	//物理内存大小
	cout << "There are " << statex.ullTotalPhys / DIV << " total KB of physical memory." << endl;
	//空闲内存大小
	cout << "There are " << statex.ullAvailPhys / DIV << " free KB of physical memory." << endl;
	//总的页文件大小
	cout << "There are " << statex.ullTotalPageFile / DIV << " total KB of paging file." << endl;
	//空闲页文件大小
	cout << "There are " << statex.ullAvailPageFile / DIV << " free KB of paging file." << endl;
	//虚拟内存大小
	cout << "There are " << statex.ullTotalVirtual / DIV << " total KB of virtual memory." << endl;
	//可用的虚拟内存大小
	cout << "There are " << statex.ullAvailVirtual / DIV << " free  KB of virtual memory." << endl;

	//来自书本P286
	//获得系统信息
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);

	cout << "-------------------statement of system-------------------" << endl;
	//虚拟内存页大小
	cout << "Virtual memory page size: " << si.dwPageSize / DIV << "KB" << endl;
	cout << "Minimum application address: " << si.lpMinimumApplicationAddress << endl;
	cout << "Maximum application address: " << si.lpMaximumApplicationAddress << endl;
	return 0;
}