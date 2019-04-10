#include "stdafx.h"
#include "iostream"
#include <Windows.h>
#include "string"

using namespace std;

void copySingleFile(const char *file_source, const char *file_target)//复制单个文件
{
	WIN32_FIND_DATA lpfindfiledata;
	//查找指定文件路径
	HANDLE h_find = FindFirstFile(file_source,//文件名
		&lpfindfiledata);//数据缓冲区

	HANDLE h_source = CreateFile(file_source,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	HANDLE h_target = CreateFile(file_target,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	WIN32_FIND_DATA time_source;
	GetFileTime(h_source, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);

	LONG size = lpfindfiledata.nFileSizeLow - lpfindfiledata.nFileSizeHigh;

	DWORD wordbit;

	//新开缓冲区，保存数据
	int *BUFFER = new int[size];
	//源文件读数据
	ReadFile(h_source,
		BUFFER,
		size,
		&wordbit,
		NULL);
	//目标文件写数据
	WriteFile(h_target,
		BUFFER,
		size,
		&wordbit,
		NULL);

	SetFileTime(h_target, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);

	CloseHandle(h_find);
	CloseHandle(h_source);
	CloseHandle(h_target);
}

void copyDir(const char *d_source, const char *d_target)
{
	WIN32_FIND_DATA lpfindfiledata;
	string f_source = d_source;
	string f_target = d_target;

	f_source.append("\\*.*");
	f_target.append("\\");

	HANDLE hfind = FindFirstFile(f_source.data(),//文件名
		&lpfindfiledata);//数据缓冲区
	if (hfind != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hfind, &lpfindfiledata) != 0)//循环查找FindFirstFile()函数搜索后的下一个文件
		{
			//查找下一个文件成功
			if ((lpfindfiledata.dwFileAttributes) == 16)//判断是否为目录s
			{
				if ((strcmp(lpfindfiledata.cFileName, ".") != 0) && (strcmp(lpfindfiledata.cFileName, "..") != 0))
				{
					f_source.clear();
					f_source.append(d_source).append("\\").append(lpfindfiledata.cFileName);
					f_target.append(lpfindfiledata.cFileName);//追加文件

					CreateDirectory(f_target.data(), NULL);//为目标文件创建目录
					copyDir(f_source.data(), f_target.data());//进入子目录复制

					HANDLE h_source = CreateFile(f_source.data(),
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_BACKUP_SEMANTICS,
						NULL);
					HANDLE h_target = CreateFile(f_target.data(),
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_BACKUP_SEMANTICS,
						NULL);

					WIN32_FIND_DATA time_source;
					GetFileTime(h_source, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);
					SetFileTime(h_target, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);

					f_source.clear();
					f_source.append(d_source).append("\\");
					f_target.clear();
					f_target.append(d_target).append("\\");
				}
			}
			else
			{
				f_source.clear();
				f_source.append(d_source).append("\\").append(lpfindfiledata.cFileName);
				f_target.append(lpfindfiledata.cFileName);//追加文件

				copySingleFile(f_source.data(),f_target.data());//直接调用文件复制函数

				f_source.clear();
				f_source.append(d_source).append("\\");
				f_target.clear();
				f_target.append(d_target).append("\\");
			}
		}
	}
	else
	{
		cout << "查找指定文件路径的文件失败!" << endl;
	}
}

int main(int argc,char* argv[])
{
	if (argc != 3) //判断参数是否合理
	{
		cout << "Error Arguments" << endl;
		return 0;
	}
	else
	{
		WIN32_FIND_DATA lpfindfiledata;
		if (FindFirstFile(argv[1], &lpfindfiledata) == INVALID_HANDLE_VALUE)
		{
			printf("查找源文件路径失败!\n");
		}
		if (FindFirstFile(argv[2], &lpfindfiledata) == INVALID_HANDLE_VALUE)
		{
			CreateDirectory(argv[2], NULL);//为目标文件创建目录
		}
		copyDir(argv[1], argv[2]);

		HANDLE h_source = CreateFile(argv[1],
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);
		HANDLE h_target = CreateFile(argv[2],
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);

		WIN32_FIND_DATA time_source;
		GetFileTime(h_source, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);
		SetFileTime(h_target, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);
	}
	printf("复制完成!\n");
	return 0;
}