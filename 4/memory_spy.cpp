#include "stdafx.h"

#include <cstdio>
#include <cstdlib>
#include <iostream> 
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <iomanip>
#pragma comment(lib,"Shlwapi.lib")
#define DIV 1024
using namespace std;

//显示保护标记，该标记表示允许应用程序对内存进行访问的类型
inline bool TestSet(DWORD dwTarget, DWORD dwMask)
{
	return ((dwTarget &dwMask) == dwMask);
}

#define SHOWMASK(dwTarget,type) if(TestSet(dwTarget,PAGE_##type)){cout << "," << #type;}

void showProtection(DWORD dwTarget)
{
	//定义的页面保护类型
	SHOWMASK(dwTarget, READONLY);
	SHOWMASK(dwTarget, GUARD);
	SHOWMASK(dwTarget, NOCACHE);
	SHOWMASK(dwTarget, READWRITE);
	SHOWMASK(dwTarget, WRITECOPY);
	SHOWMASK(dwTarget, EXECUTE);
	SHOWMASK(dwTarget, EXECUTE_READ);
	SHOWMASK(dwTarget, EXECUTE_READWRITE);
	SHOWMASK(dwTarget, EXECUTE_WRITECOPY);
	SHOWMASK(dwTarget, NOACCESS);
}

void getSystemInformation()
{	//来自书本P286
	//获得系统信息
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);

	cout << "-------------------System Information-------------------" << endl;
	//虚拟内存页大小
	cout << "Virtual memory page size: " << si.dwPageSize / DIV << "KB" << endl;
	//最小地址
	cout << "Minimum application address: 0x" << si.lpMinimumApplicationAddress << endl;
	//最大地址
	cout << "Maximum application address: 0x" << si.lpMaximumApplicationAddress << endl;
	cout << endl;
}

void getPerformanceInformation()
{	//https://docs.microsoft.com/zh-cn/windows/desktop/api/psapi/ns-psapi-_performance_information
	//获得存储器信息
	PERFORMANCE_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	if (GetPerformanceInfo(&pi, sizeof(pi)) == false)
	{
		cout << GetLastError() << endl;
		return;
	}
	cout << "-------------------Performence Information-------------------" << endl;
	//页面大小 以字节为单位
	cout << "The size of a page: " << pi.PageSize / DIV << "KB" << endl;
	//系统当前提交的页数
	cout << "The number of pages currently committed by the system is: " << pi.CommitTotal << endl;
	//系统可以在不扩展分页文件的情况下提交的当前最大页数
	cout << "The current maximum number of pages: " << pi.CommitLimit << endl;
	//上次重启后最大提交状态的页数
	cout << "The maximum number of pages since the last system reboot: " << pi.CommitPeak << endl;
	//实际物理内存量 以页为单位
	cout << "The amount of actual physical memory: " << pi.PhysicalTotal << endl;
	//当前可用的物理内存量 以页为单位
	cout << "The amount of physical memory currently available: " << pi.PhysicalAvailable << endl;
	//系统缓存内存量 以页为单位
	cout << "The amount of system cache memory:" << pi.SystemCache << endl;
	//分页和非分页内核池中当前内存的总和 以页为单位
	cout << "The sum of the memory currently in the paged and nonpaged kernel pools: " << pi.KernelTotal << endl;
	//当前在分页内核池中的内存 以页为单位
	cout << "The memory currently in the paged kernel pool: " << pi.KernelPaged << endl;
	//当前在非分页内核池中的内存 以页为单位
	cout << "The memory currently in the nonpaged kernel pool " << pi.KernelNonpaged << endl;
	//当前打开句柄的数量
	cout << "The current number of open handles: " << pi.HandleCount << endl;
	//当前打开进程的数量
	cout << "The current number of processes: " << pi.ProcessCount << endl;
	//当前打开线程的数量
	cout << "The current number of threads: " << pi.ThreadCount << endl;
	cout << endl;

}

void getMemoryInformation()
{	//来自https://docs.microsoft.com/zh-cn/windows/desktop/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	//获取物理内存信息
	if (GlobalMemoryStatusEx(&statex) == false)
	{
		cout << GetLastError() << endl;
		return;
	}
	cout << "-------------------Physical Memory Infromation-------------------" << endl;
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
	cout << endl;
}

void getProcessInformation()
{
	PROCESSENTRY32 cp;
	cp.dwSize = sizeof(cp);
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //进程快照
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		cout << "Create snapshot error" << endl;
		return;
	}

	cout << "-------------------Process Information-------------------" << endl;

	bool bMore = Process32First(hProcess, &cp);
	while (bMore)
	{
		cout << "PID: " << cp.th32ParentProcessID << " Name: " << cp.szExeFile << endl;
		bMore = Process32Next(hProcess, &cp);
	}

	CloseHandle(hProcess);
}

void getProcessInfoByID()
{
	int PID;
	cout << "-------------------Get process information by ID-------------------" << endl;
	cout << "Enter PID:";
	cin >> PID;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	SYSTEM_INFO si;	//系统信息结构
	ZeroMemory(&si, sizeof(si));	//初始化
	GetSystemInfo(&si);	//获得系统信息

	MEMORY_BASIC_INFORMATION mbi;	//进程虚拟内存空间的基本信息结构
	ZeroMemory(&mbi, sizeof(mbi));	//分配缓冲区，用于保存信息

									//循环整个应用程序地址空间
	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress;
	while (pBlock < si.lpMaximumApplicationAddress)
	{
		//获得下一个虚拟内存块的信息
		if (VirtualQueryEx(
			hProcess,	//相关的进程
			pBlock,		//开始位置
			&mbi,		//缓冲区
			sizeof(mbi)) == sizeof(mbi))	//长度的确认，如果失败返回0
		{
			//计算块的结尾及其长度
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			TCHAR szSize[MAX_PATH];
			//将数字转换成字符串
			StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH);

			//显示块地址和长度
			cout.fill('0');
			cout << hex << setw(8) << (DWORD)pBlock << "-" << hex << setw(8) << (DWORD)pEnd << (strlen(szSize) == 7 ? "(" : "(") << szSize << ")";

			//显示块的状态
			switch (mbi.State)
			{
			case MEM_COMMIT:
				printf("Commited");
				break;
			case MEM_FREE:
				printf("Free");
				break;
			case MEM_RESERVE:
				printf("Reserved");
				break;
			}

			//显示保护
			if (mbi.Protect == 0 && mbi.State != MEM_FREE)
			{
				mbi.Protect = PAGE_READONLY;
			}
			showProtection(mbi.Protect);

			//显示类型
			switch (mbi.Type)
			{
			case MEM_IMAGE:
				printf(", Image");
				break;
			case MEM_MAPPED:
				printf(", Mapped");
				break;
			case MEM_PRIVATE:
				printf(", Private");
				break;
			}

			//检验可执行的映像
			TCHAR szFilename[MAX_PATH];
			if (GetModuleFileName(
				(HMODULE)pBlock,			//实际虚拟内存的模块句柄
				szFilename,					//完全指定的文件名称
				MAX_PATH) > 0)				//实际使用的缓冲区长度
			{
				//除去路径并显示
				PathStripPath(szFilename);
				printf(", Module:%s", szFilename);
			}

			printf("\n");
			//移动块指针以获得下一个块
			pBlock = pEnd;
		}
	}
}

int main()
{
	getSystemInformation();		//获取系统信息
	getPerformanceInformation();	//获取性能信息
	getMemoryInformation();		//获取存储器信息
	getProcessInformation();	//获取进程信息
	getProcessInfoByID();	//根据PID获取该进程的虚拟地址空间和工作集
	return 0;
}
