#ifndef __CPU_H__
#define __CPU_H__
#include"type.h"
#include<stdio.h>
#include<sys/types.h>

/*宏定义*************************************************/
#define CPU_BUF_SIZE 64     //CPU每条信息的容量


/*结构体定义*************************************************/
/**
 * @brief cpu硬件信息
 * 
 */
typedef struct {
    char vendorID[CPU_BUF_SIZE];       //CPU制造商
    char modelName[CPU_BUF_SIZE];      //记录CPU属于的名字及其编号、标称主频
    char cpuMHz[CPU_BUF_SIZE];         //CPU的实际使用主频
    char cacheSize[CPU_BUF_SIZE];      //CPU二级缓存大小
}CPU_INFO;


/**
 * @brief 存储CPU状态信息的结构体;
 * 单位 USER_HZ:10ms;
 * CPU使用率 = 1- (空闲时间/总CPU时间)
 */
typedef struct 
{
    unsigned long user;     //处于用户态的运行时间，不包含 nice值为负进程
    unsigned long nice;     //nice值为负的进程所占用的CPU时间
    unsigned long system;   //处于核心态的运行时间
    unsigned long idle;     //除IO等待时间以外的其它等待时间iowait
    unsigned long iowait;   //IO等待时间
    unsigned long irq;      //硬中断时间
    unsigned long softirq;  //软中断时间
    unsigned long steal;    //系统运行在虚拟机中的时候，被其他虚拟机占用的 CPU 时间
    unsigned long guest;        //通过虚拟化运行其他操作系统的时间，也就是运行虚拟机的 CPU 时间
    unsigned long guest_nice;   //以低优先级运行虚拟机的时间
    unsigned long total;    //总CPU时间
}CPU_STAT_INFO;

/**
 * @brief 进程cpu信息
 * 
 */
typedef struct 
{
    pid_t pid;              //进程pid
    unsigned long utime;    //用户态运行的时间
    unsigned long stime;    //核心态运行的时间
    unsigned long cutime;   //所有已死线程在用户态运行的时间
    unsigned long cstime;   //所有已死在核心态运行的时间
    unsigned long total;    //进程总cpu时间
}PROCESS_CPU_STAT_INFO;


/*函数定义*************************************************/

CPU_INFO * getCpuInfo(const char *filename);

boolean destoryCpuInfo(CPU_INFO **info);

CPU_STAT_INFO * getCpuStatInfo(const char *filename);

boolean destoryCpuStatInfo(CPU_STAT_INFO **info);

float getCpuPrecent(CPU_STAT_INFO *newInfo,CPU_STAT_INFO *oldInfo);

PROCESS_CPU_STAT_INFO * getProcessCpuStatInfo(pid_t pid);

boolean destoryProcessCpuStatInfo(PROCESS_CPU_STAT_INFO **info);

float getProcessCpuPrecent(PROCESS_CPU_STAT_INFO *newProcess,PROCESS_CPU_STAT_INFO *oldProcess,CPU_STAT_INFO *newCpu,CPU_STAT_INFO *oldCpu);

#endif
