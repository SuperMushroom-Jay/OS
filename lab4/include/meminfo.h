#ifndef __MEMINFO_H__
#define __MEMINFO_H__
#include "type.h"
#include <stdio.h>
#include <sys/types.h>

#define MEM_BUF_SIZE 64

/*结构体定义*************************************************/
/**
 * @brief 记录内存信息
 *
 */
typedef struct
{
    unsigned long MemTotal;     //总内存大小
    unsigned long MemFree;      //空闲内存大小
    unsigned long MemAvailable; //可用内存大小
    unsigned long Buffers;      //磁盘缓存的大小
    unsigned long Cached;       //可用内存大小
} MEM_INFO;

/*函数定义*************************************************/

MEM_INFO *getMemInfo(const char * filename);

boolean destoryMemInfo(MEM_INFO **info);

float getMemPrecent(MEM_INFO *info);

float getProcessMem(pid_t pid);

#endif