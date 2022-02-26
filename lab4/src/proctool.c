#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"proctool.h"
#include"stringtool.h"

CPU_INFO * get_cpu_info(char *filename){
    FILE * fp;
    fp=fopen("/proc/cpuinfo","r");  //以只读模式读出cpuinfo中的信息
    if(fp==NULL){
        printf("/proc/cpuinfo open failed.\n");
        return NULL;
    }
    char buf[CPU_BUF_SIZE];
    CPU_INFO * cpu_info=(CPU_INFO *)malloc(sizeof(CPU_INFO));   //开创CPU_INFO空间接收变量值
    int count = 0;
    while ( fgets(buf,64,fp)!=NULL && count<4 )  //从cpuinfo文件中一行一行读取信息
    {
        switch (buf[0])
        {
        case 'v':{
            if(string_match(buf,"^vendor_id")==TRUE){   //若字符匹配，则将buf中的内容拷贝
                memcpy(cpu_info->vendor_id,buf,strlen(buf));
                count++;
            }
        }break;
        case 'm':{
            if(string_match(buf,"^model name")==TRUE){
                memcpy(cpu_info->model_name,buf,strlen(buf));
                count++;
            }
        }break;
        case 'c':{
            if(string_match(buf,"^cpu MHz")==TRUE){
                memcpy(cpu_info->cpu_mhz,buf,strlen(buf));
                count++;
            }
            if(string_match(buf,"^cache size")==TRUE){
                memcpy(cpu_info->cache_size,buf,strlen(buf));
                count++;
            }
        }break;
        default:
            break;
        }
    }
    fclose(fp);
    return cpu_info;
}

