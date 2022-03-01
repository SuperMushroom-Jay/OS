#ifndef __PROCESS_H__
#define __PROCESS_H__
#include"type.h"
#include<stdio.h>
#include<sys/types.h>

/*宏定义*************************************************/
#define PRO_BUF_SIZE 32     //PROCESS每条信息容量


/*结构体定义*************************************************/
typedef struct {
    pid_t pid;                          //进程号
    char comm[PRO_BUF_SIZE];            //进程名
    char task_state;                    //进程任务状态
    pid_t ppid;                         //父进程号
    pid_t pgid;                         //线程组号
    char user[PRO_BUF_SIZE];            //所属用户
    float memery;                       //占用内存
    float cpu;                          //cpu使用率
}PROCESS_INFO;

/*函数定义*************************************************/
PROCESS_INFO * getPidInfo(pid_t pid);

#endif