#include"type.h"
#include"meminfo.h"
#include"stringtool.h"
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/**
 * @brief 获取内存的信息
 * 
 * @param filename /proc/meminfo
 * @return MEM_INFO* 
 */
MEM_INFO * getMemInfo(const char * filename){
    FILE *fp;
    fp=fopen(filename,"r");
    if(fp==NULL){
        return NULL;
    }
    char tempStr[10];
    MEM_INFO *info=(MEM_INFO *)malloc(sizeof(MEM_INFO));

    fscanf(fp,"%s%ld%s",tempStr,&info->MemTotal,tempStr);
    fscanf(fp,"%s%ld%s",tempStr,&info->MemFree,tempStr);
    fscanf(fp,"%s%ld%s",tempStr,&info->MemAvailable,tempStr);
    fscanf(fp,"%s%ld%s",tempStr,&info->Buffers,tempStr);
    fscanf(fp,"%s%ld%s",tempStr,&info->Cached,tempStr);
    fclose(fp);
    // printf("%ld\n",info->MemTotal);
    // printf("%ld\n",info->MemAvailable);

    return info;
}

/**
 * @brief 获取内存使用率
 * 
 * @param info 内存信息
 * @return float 
 */
float getMemPrecent(MEM_INFO *info){
    float tmpNum=info->MemTotal-info->MemAvailable;
    // printf("tmpNum = %.2f\n",tmpNum);
    float precent=(tmpNum/info->MemTotal)*100;
    // printf("precent = %.2f\n",precent);
    return precent;
}

/**
 * @brief 获取进程的运行内存
 * 
 * @param pid 进程pid
 * @return float 
 */
float getProcessMem(pid_t pid){
    char filename[MEM_BUF_SIZE];
    sprintf(filename,"/proc/%d/status",pid);
    FILE *fp;
    fp=fopen(filename,"r");
    if(fp==NULL){
        return 0;
    }
    char tempStr[MEM_BUF_SIZE];
    int index=0;
    float VmRSS;
    while (fgets(tempStr,MEM_BUF_SIZE,fp)!=NULL)    //定位到VmRss
    {
        if(stringMatch(tempStr,"^VmRSS")){
            printf("YES");
            break;
        }
    }
    sscanf(tempStr,"%s%f",filename,&VmRSS);
    //printf("tempStr : %s\n",tempStr);
    VmRSS=VmRSS/1024;
    return VmRSS;   //返回占用内存 xx MB
}
