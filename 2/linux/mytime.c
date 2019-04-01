#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
int glob = 3;
struct timeval tv_begin, tv_end;

void printTime(long int totaltime)
{
    long int hour = totaltime / (3600000000);
    long int hour_temp = totaltime % (3600000000);

    long int min = hour_temp / (60000000);
    long int min_temp = hour_temp % (60000000);

    long int sec = min_temp / 1000000;
    long int sec_temp = min_temp % 1000000;

    long int msec = sec_temp / 1000;
    long int usec = sec_temp % 1000;

    printf("The time is : %ld小时%ld分%ld秒%ld毫秒%ld微秒 \n", hour, min, sec, msec, usec);
}

void createTestProcess(char *filename)
{ //运行
    __pid_t fpid;
    gettimeofday(&tv_begin, NULL);
    fpid = fork();
    if (fpid < 0)
    {
        printf("fork() error \n");
        _exit(0);
    }
    else if (fpid == 0)
    { //子进程
        if (strcmp(filename, "test"))
        {
            if (execlp(filename, "", NULL) < 0) //运行系统应用程序
            {
                printf("execle error\n");
            }
        }
        else
        {
            if (execlp("/home/zhangwuen/os/test", filename, NULL) < 0) //运行指定进程
            {
                printf("execle error\n");
            }
        }
    }

    wait(0);

    gettimeofday(&tv_end, NULL);
    float diff = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + tv_end.tv_usec - tv_begin.tv_usec;
    int totaltime = (int)diff;
    printTime(totaltime);
    _exit(0);
}

void createLimitProcess(char *filename, char *runtime)
{
    __pid_t fpid;
    gettimeofday(&tv_begin, NULL);
    fpid = fork();
    if (fpid < 0)
    {
        printf("fork() error \n");
        _exit(0);
    }
    else if (fpid == 0)
    {
        if (execlp("/home/zhangwuen/os/test", "test", NULL) < 0)
        {
            printf("execle error\n");
        }
    }
    else
    {
        //父进程执行相应时间
        sleep(atoi(runtime));

        gettimeofday(&tv_end, NULL);
        float diff = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + tv_end.tv_usec - tv_begin.tv_usec;
        int totaltime = (int)diff;
        printTime(totaltime);

        //父进程结束
        _exit(0);
    }

    wait(0);

    gettimeofday(&tv_end, NULL);
    float diff = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + tv_end.tv_usec - tv_begin.tv_usec;
    int totaltime = (int)diff;
    printTime(totaltime);
    _exit(0);
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        createTestProcess(argv[1]);
    }
    else
    {
        createLimitProcess(argv[1], argv[2]);
    }

    return 0;
}