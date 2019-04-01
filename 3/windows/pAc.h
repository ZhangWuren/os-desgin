#ifndef PAC_H
#define PAC_H

#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<tchar.h>
#include<ctime>

#define buffer_size 3

#define producer_num 2
#define consumer_num 3

#define produce_times 6
#define consume_times 4

#define empty _T("empty")
#define full _T("full")
#define mutex _T("mutex")

#define buffer _T("buffer")

#define P(S) WaitForSingleObject(S, INFINITE)
#define V(S) ReleaseSemaphore(S, 1, NULL)

typedef struct{
	int arr[buffer_size];
	int number;
	int count;
}bufferArr;

typedef struct {
	bufferArr bufferarr;
	HANDLE s_empty;
	HANDLE s_full;
	HANDLE s_mutex;
}shareBuffer;

//创建共享数据空间
HANDLE MakeSharedFile()
{
	//创建文件映射对象
	HANDLE hMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,	//使用页式临时文件
		NULL,	//默认的安全性
		PAGE_READWRITE,	//读写权
		0,	//文件最大尺寸 高32位
		sizeof(shareBuffer),	//文件最大尺寸 低32位
		buffer	//名字
	);

	if (hMapping != INVALID_HANDLE_VALUE)
	{
		//在文件映射上创建视图
		LPVOID pData = MapViewOfFile(
			hMapping,	//文件映射对象的对象
			FILE_MAP_ALL_ACCESS,	//获得读写权
			0,	//在文件的开头处 高32位开始
			0,	//在文件的开头处 低32位开始
			0	//映射整个文件
		);

		if (pData != NULL)
		{
			ZeroMemory(pData, sizeof(shareBuffer));	//分配内存空间，并清0
		}

		UnmapViewOfFile(pData);	//撤销文件映射对象
	}

	return hMapping;
}

int getRandomInt()
{
	srand(time(0));
	return rand() % 10;
}

void show(bufferArr bufferarr)
{
	printf("缓冲区中内容为:");
	if (bufferarr.number == 0) {
		printf("空\n");
	}
	else {
		for (int i = 0; i < bufferarr.number; i++)
		{
			printf("%d ", bufferarr.arr[i]);
		}
		printf("\n");
	}
}
#endif