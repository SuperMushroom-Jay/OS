#ifndef __PROCTOOL_H__
#define __PROCTOOL_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*宏定义*************************************************/
#define TRUE 1
#define FALSE 0
#define CPU_BUF_SIZE 64     //CPU每条信息的容量

/*结构体定义*************************************************/
typedef struct {
    char vendor_id[CPU_BUF_SIZE];       //CPU制造商
    char model_name[CPU_BUF_SIZE];      //记录CPU属于的名字及其编号、标称主频
    char cpu_mhz[CPU_BUF_SIZE];         //CPU的实际使用主频
    char cache_size[CPU_BUF_SIZE];      //CPU二级缓存大小
}CPU_INFO;

/*函数定义*************************************************/
char* get_cpu_info(char *filename);     //获取CPU 的信息(型号, 家族, 缓存大小等)





#endif