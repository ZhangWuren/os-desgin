#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>

#define producer_num 2
#define consumer_num 3

#define produce_times 6
#define consume_times 4

#define s_mutex 0
#define s_empty 1
#define s_full 2

#define buffer_size 3

#define SHMKEY 75

using namespace std;

int shmid;

typedef struct
{
    int arr[buffer_size];
    int number;
    int count;
} buffer;

//P操作
void P(int sem_set_id)
{
    struct sembuf sem_op;
    sem_op.sem_num = 0;            //信号量集合中要操作的信号量的索引
    sem_op.sem_op = -1;            //信号量的操作值为-1 即P操作
    sem_op.sem_flg = 0;            //访问标志
    semop(sem_set_id, &sem_op, 1); //调用语法
}

//V操作
void V(int sem_set_id)
{
    struct sembuf sem_op;
    sem_op.sem_num = 0;            //信号量集合中要操作的信号量的索引
    sem_op.sem_op = 1;             //信号量的操作值为+1 即V操作
    sem_op.sem_flg = 0;            //访问标志
    semop(sem_set_id, &sem_op, 1); //调用语法
}

int getRandomInt()
{
    srand((int)time(NULL));
    return (rand() % 100 + 1);
}

void getNowTime()
{
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time); //获取相对于1970到现在的秒数
    tm nowTime;
    localtime_r(&time.tv_sec, &nowTime);
    char current[1024];
    sprintf(current, "%04d/%02d/%02d %02d:%02d:%02d", nowTime.tm_year + 1900, nowTime.tm_mon + 1, nowTime.tm_mday,
            nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
    cout << "当前时间为:" << current << endl;
}

void show(buffer *bufferarr)
{
    cout << "缓冲区内容为：";
    if (bufferarr->number == 0)
    {
        cout << "空" << endl;
    }
    else
    {
        for (int i = 0; i < bufferarr->number; i++)
        {
            cout << bufferarr->arr[i] << " ";
        }
        cout << endl;
    }
}

void runProducer(int s_mutex_id, int s_empty_id, int s_full_id)
{
    pid_t pid_producer;

    pid_producer = fork();
    if (pid_producer < 0)
    {
        cout << ("Fork error") << endl;
        return;
    }

    if (pid_producer == 0)
    { //进入生产者子进程
        buffer *shmptr = (buffer *)shmat(shmid, 0, 0);
        if (shmptr == (void *)-1)
        {
            cout << "Shmat error in Producer " << getpid() << endl;
            return;
        }
        for (int i = 0; i < produce_times; i++)
        {
            sleep(i % 3);
            int data = getRandomInt();
            P(s_empty_id);
            P(s_mutex_id);
            //生产者操作
            shmptr->arr[shmptr->number] = data;
            shmptr->number++;
            shmptr->count++;
            printf("\n第%d次操作:\n", shmptr->count);
            getNowTime();
            cout << getpid() << " 号生产者进程将 " << data << " 放入buffer中" << endl;
            cout << "buffer的size为 " << shmptr->number << endl;
            show(shmptr);

            V(s_full_id);
            V(s_mutex_id);
        }

        shmdt(shmptr);
        exit(0);
    }
}

void runConsumer(int s_mutex_id, int s_empty_id, int s_full_id)
{
    pid_t pid_consumer;

    pid_consumer = fork();
    if (pid_consumer < 0)
    {
        cout << ("Fork error") << endl;
        return;
    }

    if (pid_consumer == 0)
    { //进入消费者子进程
        buffer *shmptr = (buffer *)shmat(shmid, 0, 0);
        if (shmptr == (void *)-1)
        {
            cout << "Shmat error in Producer " << getpid() << endl;
            return;
        }
        for (int i = 0; i < consume_times; i++)
        {
            sleep(i % 3);
            P(s_full_id);
            P(s_mutex_id);
            //生产者操作
            shmptr->arr[shmptr->number] = 0;
            shmptr->number--;
            shmptr->count++;
            int data = shmptr->arr[shmptr->number];
            printf("\n第%d次操作:\n", shmptr->count);
            getNowTime();
            cout << getpid() << " 号消费者进程将 " << data << " 从buffer中取出" << endl;
            cout << "buffer的size为 " << shmptr->number << endl;
            show(shmptr);

            V(s_empty_id);
            V(s_mutex_id);
        }

        shmdt(shmptr);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    int sem_set_id;

    //创建mutex信号量并赋值
    int s_mutex_id = semget(s_mutex, 1, IPC_CREAT | 0600);
    if (semctl(s_mutex_id, 0, SETVAL, 1) == -1) //对信号量执行控制操作(信号量集合标识，信号量的索引，要执行的操作命令，设置或返回信号量的参数)
    {
        cout << "11" << endl;
        perror("main:setctl");
        exit(1);
    }

    //创建empty信号量并赋值
    int s_empty_id = semget(s_empty, 1, IPC_CREAT | 0600);
    if (semctl(s_empty_id, 0, SETVAL, buffer_size) == -1)
    {
        cout << "22" << endl;
        perror("main:setctl");
        exit(1);
    }

    //创建full信号量并赋值
    int s_full_id = semget(s_full, 1, IPC_CREAT | 0600);
    if (semctl(s_full_id, 0, SETVAL, 0) == -1)
    {
        cout << "33" << endl;
        perror("main:setctl");
        exit(1);
    }

    //申请一个共享内存区
    shmid = shmget(
        IPC_PRIVATE,       //共享内存区的关键字
        sizeof(buffer),    //共享内存区的大小，这里为缓冲区的大小
        0700 | IPC_CREAT); //创建标志
    if (shmid < 0)
    {
        cout << "Shmget error" << endl;
        exit(1);
    }

    buffer *shmptr = (buffer *)shmat(shmid, 0, 0); //将共享段附加到申请通信的进程空间
    //第二个参数shmadd=0，表示将该共享段附加到系统选择的进程的第一个可用地址之后
    //第三个参数表示允许对共享段的访问方式
    //成功则返回共享内存附加到进程地址空间的虚地址，失败返回-1

    if (shmptr == (void *)-1)
    {
        cout << "Shmat error" << endl;
        exit(1);
    }
    else
    {
        //初始化buffer内容
        shmptr->count = 0;
        shmptr->number = 0;
        shmdt(shmptr); //将共享段与进程之间解除连接
    }

    //生产者进程
    for (int i = 0; i < producer_num; i++)
    {
        runProducer(s_mutex_id, s_empty_id, s_full_id);
    }

    //消费者进程
    for (int i = 0; i < consumer_num; i++)
    {
        runConsumer(s_mutex_id, s_empty_id, s_full_id);
    }

    for (int i = 0; i < producer_num + consumer_num; i++)
    {
        int child_status;
        wait(&child_status);
    }
    cout << endl;
    cout << "运行结束" << endl;
    semctl(s_empty_id, 0, IPC_RMID, 0); //IPC_RMID 删除信号量
    semctl(s_full_id, 0, IPC_RMID, 0);
    semctl(s_mutex_id, 0, IPC_RMID, 0);
}