#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include"cpu.h"
#include"type.h"
#include"stringtool.h"

/**
 * @brief 获取cpu的简单信息
 * 
 * @param filename cpuinfo文件路径 /proc/cpuinfo
 * @return CPU_INFO* 返回CPU_INFO指针
 */
CPU_INFO * getCpuInfo(const char *filename){
    FILE * fp;
    fp=fopen(filename,"r");  //以只读模式读出cpuinfo中的信息
    if(fp==NULL){
        //printf("/proc/cpuinfo open failed.\n");
        return NULL;
    }
    char buf[CPU_BUF_SIZE];
    CPU_INFO * cpuInfo=(CPU_INFO *)malloc(sizeof(CPU_INFO));   //开创CPU_INFO空间接收变量值
    int count = 0;
    while ( fgets(buf,64,fp)!=NULL && count<4 )  //从cpuinfo文件中一行一行读取信息
    {
        switch (buf[0])
        {
        case 'v':{
            if(stringMatch(buf,"^vendor_id")==TRUE){   //若字符匹配，则将buf中的内容拷贝
                memcpy(cpuInfo->vendorID,buf,strlen(buf));
                count++;
            }
        }break;
        case 'm':{
            if(stringMatch(buf,"^model name")==TRUE){
                memcpy(cpuInfo->modelName,buf,strlen(buf));
                count++;
            }
        }break;
        case 'c':{
            if(stringMatch(buf,"^cpu MHz")==TRUE){
                memcpy(cpuInfo->cacheSize,buf,strlen(buf));
                count++;
            }
            if(stringMatch(buf,"^cache size")==TRUE){
                memcpy(cpuInfo->cacheSize,buf,strlen(buf));
                count++;
            }
        }break;
        default:
            break;
        }
    }
    fclose(fp);
    return cpuInfo;
}

/**
 * @brief 获取cpu的状态信息
 * 
 * @param filename /proc/stat
 * @return CPU_STAT_INFO* 返回指针
 */
CPU_STAT_INFO * getCpuStatInfo(const char *filename){
    FILE *fp;
    fp=fopen(filename,"r");
    if(fp==NULL){
        return NULL;
    }
    char tempStr[10];
    CPU_STAT_INFO *info=(CPU_STAT_INFO *)malloc(sizeof(CPU_STAT_INFO));
    fscanf(fp,"%s%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld",tempStr,
            &info->user,&info->nice,&info->system,&info->idle,
            &info->iowait,&info->irq,&info->softirq,&info->steal,
            &info->guest,&info->guest_nice);
    fclose(fp);
    info->total=info->user+info->nice+info->system+
                info->idle+info->iowait+info->irq+
                info->softirq+info->steal+info->guest+
                info->guest_nice;
    // printf("idle = %ld\n",info->idle);
    // printf("total = %ld\n",info->total);
    return info;
}

/**
 * @brief 获取cpu效率
 * 
 * @param newInfo 
 * @param oldInfo 
 * @return float 返回计算的效率，保留两位小数
 */
float getCpuPrecent(CPU_STAT_INFO *newInfo,CPU_STAT_INFO *oldInfo){
    float idle=newInfo->idle - oldInfo->idle;
    float total=newInfo->total - oldInfo->total;
    float precent=(1-idle/total)*100;
    //printf("idle=%.2f total=%.2f\n",idle,total);
    return precent;
}

/**
 * @brief 获取进程的cpu时间
 * 
 * @param pid 进程pid
 * @return PROCESS_CPU_STAT_INFO* 返回指针
 */
PROCESS_CPU_STAT_INFO * getProcessCpuStatInfo(pid_t pid){
    char filename[30];
    sprintf(filename,"/proc/%d/stat",pid);
    FILE *fp;
    fp=fopen(filename,"r");
    if(fp==NULL){
        return NULL;
    }
    char tempStr[16];
    int index=0;
    while (index<13)    //过滤13个不需要的数据
    {
        fscanf(fp,"%s",tempStr);
        index++;
    }
    PROCESS_CPU_STAT_INFO * info = (PROCESS_CPU_STAT_INFO *)malloc(sizeof(PROCESS_CPU_STAT_INFO));
    sscanf(filename,"%s%d",tempStr,&info->pid);
    fscanf(fp,"%ld%ld%ld%ld",&info->utime,&info->stime,&info->cutime,&info->cstime);
    fclose(fp);
    info->total=info->utime+info->stime+info->cutime+info->cstime;
    //printf("Process total = %ld\n",info->total);
    return info;
}

/**
 * @brief 获取进程的cpu占用率
 * 
 * @param newProcess 进程新cpu总时间
 * @param oldProcess 进程旧cpu总时间
 * @param newCpu 新cpu总时间
 * @param oldCpu 旧cpu总时间
 * @return float 返回占用率
 */
float getProcessCpuPrecent(PROCESS_CPU_STAT_INFO *newProcess,PROCESS_CPU_STAT_INFO *oldProcess,CPU_STAT_INFO *newCpu,CPU_STAT_INFO *oldCpu){
    float processCpuTotal=newProcess->total-oldProcess->total;
    float cpuTotal=newCpu->total-oldCpu->total;
    float precent=processCpuTotal/cpuTotal*100;
    //printf("processCpuTotal=%.2f  cpuTotal=%.2f\n",processCpuTotal,cpuTotal);
    return precent;
}
