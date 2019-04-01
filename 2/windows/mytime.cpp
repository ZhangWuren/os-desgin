#include <Windows.h>
#include <iostream>
#include <string.h>
#include <tchar.h>
using namespace std;

void computeTime(SYSTEMTIME t_begin, SYSTEMTIME t_end)
{
    //默认运行时间不超过1天
    long int begin = t_begin.wHour * 60 * 60 * 1000 + t_begin.wMinute * 60 * 1000 + t_begin.wSecond * 1000 + t_begin.wMilliseconds;
    long int end = t_end.wHour * 60 * 60 * 1000 + t_end.wMinute * 60 * 1000 + t_end.wSecond * 1000 + t_end.wMilliseconds;

    long int totaltime = end - begin;

    int hour = totaltime / (60 * 60 * 1000);
    int hour_temp = totaltime % (60 * 60 * 1000);

    int minute = hour_temp / (60 * 1000);
    int minute_temp = hour_temp % (60 * 1000);

    int sec = minute_temp / 1000;
    int msec = minute_temp % 1000;

    cout << "running time is :" << hour << "h " << minute << "m " << sec << "s " << msec << "ms" << endl;
}

void createMyProcess(char *filename, char *runtime)
{
    STARTUPINFO startUpInfo; //新进程主窗口
    memset(&startUpInfo, 0, sizeof(startUpInfo));
    startUpInfo.cb = sizeof(startUpInfo);
    startUpInfo.wShowWindow = true;

    PROCESS_INFORMATION processInfo;
    memset(&processInfo, 0, sizeof(processInfo));

    bool bRet = CreateProcess(
        NULL,
        filename,
        NULL,
        NULL,
        false,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &startUpInfo,
        &processInfo);

    if (bRet)
    {
        SYSTEMTIME t_begin, t_end;
        GetSystemTime(&t_begin);
        if (strcmp(filename, runtime))
        {
            //等待子进程结束
            int time = atoi(runtime);
            time = time*1000;
            WaitForSingleObject(processInfo.hProcess, (DWORD)time);
        }
        else
        {
            //等待子进程结束
            WaitForSingleObject(processInfo.hProcess, INFINITE);
        }
        GetSystemTime(&t_end);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
        processInfo.hProcess = NULL;
        processInfo.hThread = NULL;
        processInfo.dwProcessId = 0;
        processInfo.dwThreadId = 0;
        computeTime(t_begin, t_end);
    }
    else
    {
        //如果创建进程失败，查看错误码
        DWORD dwErrCode = GetLastError();
        cout << "ErrCode :" << dwErrCode << endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        createMyProcess(argv[1], argv[1]);
    }
    else
    {
        createMyProcess(argv[1], argv[2]);
    }

    return 0;
}