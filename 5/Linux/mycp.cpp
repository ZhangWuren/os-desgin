#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <dirent.h>
#include <string.h>
#include <string>
using namespace std;

void copySingleFile(const char *file_source, const char *file_target) //复制单个文件
{
    int f_source, f_target, n;
    char buf[BUFSIZ];
    struct stat statbuff;
    struct utimbuf timebuff;
    //打开源文件
    if ((f_source = open(file_source, O_RDONLY)) == -1)
    {
        cout << "Can't open " << file_source << endl;
        return;
    }

    //将源文件信息放入statbuff中
    stat(file_source, &statbuff);

    //创建目标文件
    if ((f_target = creat(file_target, statbuff.st_mode)) == -1)
    {
        cout << "Can't create " << file_source << endl;
        return;
    }

    //复制文件
    while ((n = read(f_source, buf, BUFSIZ)) > 0)
    {
        if (write(f_target, buf, n) != n)
        {
            cout << "Write Error" << endl;
        }
    }

    timebuff.actime = statbuff.st_atime;
    timebuff.modtime = statbuff.st_mtime;
    utime(file_target, &timebuff);

    close(f_source);
    close(f_target);
}

void copyDir(const char *d_source, const char *d_target)
{
    struct stat statbuff;
    struct utimbuf timebuff;
    struct dirent *entry;
    DIR *dp;
    string dir_source = d_source;
    string dir_target = d_target;

    //打开目录
    dp = opendir(dir_source.data());
    //读取目录
    while ((entry = readdir(dp)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        { //如果是以"."或".."开头，则继续读
            continue;
        }
        if (entry->d_type == DT_DIR)
        {                                                 //如果读到的类型为4，即为目录，则复制目录
            dir_source.append("/").append(entry->d_name); //拼接路径
            dir_target.append("/").append(entry->d_name);

            cout << dir_source.data() << endl;
            cout << dir_target.data() << endl;

            stat(dir_source.data(), &statbuff);         //将dir_source信息放入statbuff中
            mkdir(dir_target.data(), statbuff.st_mode); //创建新目录并且给予权限
            timebuff.actime = statbuff.st_atime;        //复制创建和修改时间
            timebuff.modtime = statbuff.st_mtime;
            utime(dir_target.data(), &timebuff);
            //cout<<statbuff.st_atime<<endl;
            //递归
            copyDir(dir_source.data(), dir_target.data());
            dir_source = d_source;
            dir_target = d_target;
        }
        else
        {
            dir_source.append("/").append(entry->d_name);
            dir_target.append("/").append(entry->d_name);
            copySingleFile(dir_source.data(), dir_target.data());
            dir_source = d_source;
            dir_target = d_target;
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc != 3) //判断参数是否合理
    {
        cout << "Error Arguments" << endl;
        return 0;
    }

    DIR *dir;

    char *dir_source = argv[1];
    char *dir_target = argv[2];

    struct stat statbuff;    //文件数据结构
    struct utimbuf timebuff; //文件时间结构

    if ((dir = opendir(dir_source)) == NULL)
    {
        cout << "Can't open source dir" << endl;
        return 0;
    }
    if ((dir = opendir(dir_target)) == NULL)
    {
        stat(dir_source, &statbuff);
        mkdir(dir_target, statbuff.st_mode);
        timebuff.actime = statbuff.st_atime;
        timebuff.modtime = statbuff.st_mtime;
        utime(dir_target, &timebuff);
    }
    copyDir(dir_source, dir_target);

    cout << "Copy Completed!" << endl;
    return 0;
}