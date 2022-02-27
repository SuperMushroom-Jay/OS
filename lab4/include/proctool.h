#ifndef __PROCTOOL_H__
#define __PROCTOOL_H__
#include<stdio.h>
#include<sys/types.h>


/*宏定义*************************************************/
#define boolean char          //定义boolean类型（布尔类型）
#define TRUE 1
#define FALSE 0
#define CPU_BUF_SIZE 64     //CPU每条信息的容量
#define SYS_BUF_SIZE 64     //SYSTEM每条信息的容量
#define PRO_BUF_SIZE 32     //PROCESS每条信息容量

/*结构体定义*************************************************/
typedef struct {
    char vendorID[CPU_BUF_SIZE];       //CPU制造商
    char modelName[CPU_BUF_SIZE];      //记录CPU属于的名字及其编号、标称主频
    char cpuMHz[CPU_BUF_SIZE];         //CPU的实际使用主频
    char cacheSize[CPU_BUF_SIZE];      //CPU二级缓存大小
}CPU_INFO;

typedef struct {
    char hostName[SYS_BUF_SIZE];       //主机名 /etc/hostname
    char bootTime[SYS_BUF_SIZE];       //系统启动的时间
    char upTime[SYS_BUF_SIZE];         //系统启动到目前为止运行的时间
    char osType[SYS_BUF_SIZE];         //系统类型
    char osRelease[SYS_BUF_SIZE];      //系统内核版本
}SYS_INFO;

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

boolean readCMD(const char* cmd,char *data, int dataLen);

CPU_INFO * getCpuInfo(const char *filename);

SYS_INFO* getSysInfo();

boolean getFileFristLine(const char *filename,char *data);

boolean getSysInfoHostName(const char *filename,SYS_INFO *sysInfo);

boolean getSysInfoBootTime(const char *filename,SYS_INFO *sysInfo);

boolean getSysInfoUpTime(const char *filename,SYS_INFO *sysInfo);

boolean getSysInfoOsType(const char *filename,SYS_INFO *sysInfo);

boolean getSysInfoOsRelease(const char *filename,SYS_INFO *sysInfo);

PROCESS_INFO * getPidInfo(pid_t pid);







#endif