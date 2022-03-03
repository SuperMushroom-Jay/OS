#ifndef __PROCESS_H__
#define __PROCESS_H__
#include"type.h"
#include"cpu.h"
#include<stdio.h>
#include<sys/types.h>

/*宏定义*************************************************/
#define PRO_BUF_SIZE 64     //PROCESS每条信息容量

/*结构体定义*************************************************/
typedef struct {
    pid_t pid;                          //进程号
    char comm[PRO_BUF_SIZE];            //进程名
    char task_state[10];                    //进程任务状态
    pid_t ppid;                         //父进程号
    pid_t pgid;                         //线程组号
    int priority;                       //进程优先级
    float memery;                       //占用内存
    float cpu;                          //cpu使用率
    PROCESS_CPU_STAT_INFO *oldCpuInfo;
}PROCESS_INFO;

/*函数定义*************************************************/

PROCESS_INFO * getProcessInfo(pid_t pid);

boolean destoryProcessInfo(PROCESS_INFO **info);

boolean updateProcessInfo(pid_t pid, PROCESS_INFO **oldProInfo, CPU_STAT_INFO *new, CPU_STAT_INFO *old);

PROCESS_INFO * searchProcessByPid(pid_t pid , PROCESS_INFO **array);

PROCESS_INFO * searchProcessByComm(const char* comm, PROCESS_INFO **array);

void initProcessInfoArray(PROCESS_INFO **array);

void getAllProcessInfo(PROCESS_INFO **array);

void updateAllProcessInfo(PROCESS_INFO **array,CPU_STAT_INFO *newCpuStat,CPU_STAT_INFO *oldCpuStat);

void destoryProcessArray(PROCESS_INFO **array);

boolean getProcessInfo_new(pid_t pid, PROCESS_INFO *info);
#endif