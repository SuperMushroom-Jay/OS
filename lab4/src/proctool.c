#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"include/proctool.h"

CPU_INFO * get_cpu_info(char *filename){
    FILE * fp;
    fp=fopen("/proc/cpuinfo","r");  //以只读模式读出cpuinfo中的信息
    if(fp==NULL){
        printf("/proc/cpuinfo open failed.\n");
        return NULL;
    }
    char buf[64];
    CPU_INFO * cpu_info=(CPU_INFO *)malloc(sizeof(CPU_INFO));
    int count = 0;
    while (fgets(buf,64,fp)!=NULL)  //从cpuinfo文件中一行一行读取信息
    {
        
    }
    return NULL;
}

