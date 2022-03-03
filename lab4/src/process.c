#include"type.h"
#include"process.h"
#include"cpu.h"
#include"meminfo.h"
#include"string.h"
#include"graph.h"
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/**
 * @brief 获取进程的初始信息
 * 
 * @param pid 进程pid
 * @return PROCESS_INFO* 
 */
PROCESS_INFO * getProcessInfo(pid_t pid){
    char path[PRO_BUF_SIZE];
    sprintf(path,"/proc/%d/stat",pid);  //获取路径信息
    FILE *fp;
    fp=fopen(path,"r");
    if(fp==NULL){
        return NULL;
    }
    PROCESS_INFO *info=(PROCESS_INFO *)malloc(sizeof(PROCESS_INFO));
    //取前面的5个数据
    fscanf(fp,"%d%s%s%d%d",&info->pid,info->comm,info->task_state,&info->ppid,&info->pgid);
    int index=5;
    int priority;
    while (index<=18)
    {
        index++;
        fscanf(fp,"%d",&priority);    //获取进程优先级
    }
    fclose(fp);         //关闭文件
    info->priority=priority;
    info->memery=getProcessMem(pid);        //获取进程内存
    // info->oldCpuInfo=getProcessCpuStatInfo(pid);
    // CPU_STAT_INFO *cpuinfo = getCpuStatInfo("/proc/stat");
    // info->cpu=info->oldCpuInfo->total/cpuinfo->total*100;  
    info->cpu=0;                          //最初创建默认cpu占用为0
    // destoryCpuStatInfo(&cpuinfo);
    return info;                        
}

/**
 * @brief 销毁进程信息内存空间
 * 
 * @param info 进程信息结构体指针
 */
boolean destoryProcessInfo(PROCESS_INFO **info){
    if(*info!=NULL){
        destoryProcessCpuStatInfo(&((*info)->oldCpuInfo));
        free(*info);
        *info=NULL;
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief 更新进程的状态信息
 * 
 * @param pid 进程pid
 * @param oldProInfo 旧进程信息
 * @param new 总的cpu新状态
 * @param old 总的cpu旧状态
 * @return boolean
 */
boolean updateProcessInfo(pid_t pid, PROCESS_INFO **oldProInfo, CPU_STAT_INFO *new, CPU_STAT_INFO *old){
    char path[PRO_BUF_SIZE];
    sprintf(path,"/proc/%d/stat",pid);  //获取路径信息
    FILE *fp;
    fp=fopen(path,"r");
    if(fp==NULL){
        destoryProcessInfo(oldProInfo);
        //printf("update open failed\n");
        return FALSE;
    }
    //printf("/proc/%d/stat can open\n",pid);
    PROCESS_INFO *newProInfo = getProcessInfo(pid);
    if(*oldProInfo == NULL){
        *oldProInfo = newProInfo;
        //printf("proArray[%d] is NULL\n",pid);
    }else{     //同一个进程
        if(strcmp((*oldProInfo)->comm,newProInfo->comm)==0){   //同名进程
            //只修改状态即可
            PROCESS_CPU_STAT_INFO *newProCpuinfo=getProcessCpuStatInfo(pid);
           // printf("proArray[%d] is comm | newProCpuinfo:%ld  oldCpuInfo:%ld\n",pid,newProCpuinfo->total,(*oldProInfo)->oldCpuInfo->total);
            (*oldProInfo)->cpu=getProcessCpuPrecent(newProCpuinfo,(*oldProInfo)->oldCpuInfo,new,old);
            destoryProcessCpuStatInfo(&((*oldProInfo)->oldCpuInfo));
            (*oldProInfo)->oldCpuInfo=newProCpuinfo;
            
        }else{
            destoryProcessInfo(oldProInfo);
            *oldProInfo = newProInfo;
           // printf("proArray[%d] is not comm\n",pid);
        }
    }
    return TRUE;
}

/**
 * @brief 通过进程数组查询pid进程
 * 
 * @param pid 进程pid
 * @param array 进程数组
 * @return PROCESS_INFO* 
 */
PROCESS_INFO * searchProcessByPid(pid_t pid , PROCESS_INFO **array){
    return array[pid];
}


/**
 * @brief 通过comm查找进程数组中的匹配项
 * 
 * @param comm  进程名
 * @param array 进程数组
 * @return PROCESS_INFO* 
 */
PROCESS_INFO * searchProcessByComm(const char* comm, PROCESS_INFO **array){
    int i;
    for(i=0;i<PID_MAX_NUMBER;i++){
        if(array[i]!=NULL&&strcmp(comm,array[i]->comm)){
            return array[i];
        }
    }
    return NULL;
}

/**
 * @brief 初始化数组为NULL
 * 
 * @param array 
 */
void initProcessInfoArray(PROCESS_INFO **array){
    memset(array,0,PID_MAX_NUMBER*sizeof(PROCESS_INFO*));
}

/**
 * @brief 获取所有进程的信息
 * 
 * @param array 进程数组（在运行的进程将存在数组中）
 */
void getAllProcessInfo(PROCESS_INFO **array){
    int i;
    for(i=1;i<PID_MAX_NUMBER;i++){
        array[i]=getProcessInfo(i);
    }
}

/**
 * @brief 获取所有进程的更新信息
 * 
 * @param array 进程数组
 * @param newCpuStat 总的新Cpu状态
 * @param oldCpuStat 总的旧Cpu状态
 */
void updateAllProcessInfo(PROCESS_INFO **array,CPU_STAT_INFO *newCpuStat,CPU_STAT_INFO *oldCpuStat){
    int i;
    for(i=1;i<PID_MAX_NUMBER;i++){
        updateProcessInfo(i,&array[i],newCpuStat,oldCpuStat);
    }
}

/**
 * @brief 销毁进程数组的空间
 * 
 * @param array 进程数组
 */
void destoryProcessArray(PROCESS_INFO **array){
    int i;
    for(i=1;i<PID_MAX_NUMBER;i++){
        if(array[i]!=NULL){
            destoryProcessInfo(&array[i]);
        }
    }
}

/**
 * @brief Get the ProcessInfo new object
 * 
 * @param info 
 */
boolean getProcessInfo_new(pid_t pid,PROCESS_INFO *info){
    char path[PRO_BUF_SIZE];
    sprintf(path,"/proc/%d/stat",pid);  //获取路径信息
    FILE *fp;
    fp=fopen(path,"r");
    if(fp==NULL){
        return FALSE;
    }
    //取前面的5个数据
    fscanf(fp,"%d%s%s%d%d",&info->pid,info->comm,info->task_state,&info->ppid,&info->pgid);
    //printf("%d %s %s %d %d \n",info->pid,info->comm,info->task_state,info->ppid,info->pgid);
    int index=5;
    int priority;
    //printf("continue file\n");
    while (index<=18)
    {
        index++;
        //printf("index = %d\n",index);
        fscanf(fp,"%d",&priority);    //获取进程优先级
    }
    //printf("end file\n");
    fclose(fp);         //关闭文件
    info->priority=priority;
    info->memery=getProcessMem(pid);        //获取进程内存
    //getProcessCpuStatInfo(pid);
    PROCESS_CPU_STAT_INFO *pcpu = getProcessCpuStatInfo(pid);
    CPU_STAT_INFO * tcpu  =getCpuStatInfo("/proc/stat");
    // CPU_STAT_INFO *cpuinfo = getCpuStatInfo("/proc/stat");
    // info->cpu=info->oldCpuInfo->total/cpuinfo->total*100;  
    info->cpu=pcpu->total/tcpu->total;                          //最初创建默认cpu占用为0
    // destoryCpuStatInfo(&cpuinfo);
    //printf("YES\n");
    destoryProcessCpuStatInfo(&pcpu);
    destoryCpuStatInfo(&tcpu);
    return TRUE;     
}