#ifndef __SYSINFO_H__
#define __SYSINFO_H__
#include"type.h"
#include<stdio.h>
#include<sys/types.h>

/*宏定义*************************************************/
#define SYS_BUF_SIZE 64     //SYSTEM每条信息的容量

/*结构体定义*************************************************/
typedef struct {
    char hostName[SYS_BUF_SIZE];       //主机名 /etc/hostname
    char bootTime[SYS_BUF_SIZE];       //系统启动的时间
    char upTime[SYS_BUF_SIZE];         //系统启动到目前为止运行的时间
    char osType[SYS_BUF_SIZE];         //系统类型
    char osRelease[SYS_BUF_SIZE];      //系统内核版本
}SYS_INFO;

/*函数定义*************************************************/

SYS_INFO* getSysInfo();

void destorySysInfo(SYS_INFO **info);

boolean getFileFristLine(const char *filename,char *data);

boolean getSysInfoHostName(const char *filename,SYS_INFO *sysInfo);

boolean getSysInfoBootTime(const char *filename,SYS_INFO *sysInfo);

boolean getSysInfoUpTime(const char *filename,SYS_INFO *sysInfo);

boolean getSysInfoOsType(const char *filename,SYS_INFO *sysInfo);

boolean getSysInfoOsRelease(const char *filename,SYS_INFO *sysInfo);



#endif