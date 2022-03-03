#include"type.h"
#include"cpu.h"
#include"process.h"
#include"meminfo.h"
#include"stat.h"
#include"graph.h"
#include"cmd.h"
#include"sysinfo.h"
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// #include"cpu.h"
// #include"type.h"
// #include<stdio.h>


boolean printInfo(PROCESS_INFO **array,const char *filename){
    FILE *fp;
    fp=fopen(filename,"w");
    if(fp==NULL){
        printf("%s open failed\n",filename);
        return FALSE;
    }
    int i;
    for(i=1;i<PID_MAX_NUMBER;i++){
        if(array[i]!=NULL){
            PROCESS_INFO *info=array[i];
            fprintf(fp,"pid %d comm %s mem %.2f cpu %.2f\n",info->pid,info->comm,info->memery,info->cpu);
        }
    }
    fclose(fp);
    return TRUE;
}


int main(int argc,char **argv){
    // SYS_INFO *sys_info=getSysInfo();
    // if(sys_info!=NULL){
    //     printf("%s\n",sys_info->hostName);
    //     printf("%s\n",sys_info->bootTime);
    //     printf("%s\n",sys_info->upTime);
    //     printf("%s\n",sys_info->osType);
    //     printf("%s\n",sys_info->osRelease);
    // }else{
    //     printf("FALSE");
    // }
    // char buf[1024];
    // readCMD("date -d \"$(awk -F. '{print $1}' /proc/uptime) second ago\" +\"%Y-%m-%d %H:%M:%S\"",buf,1024);
    // boolean status = getSysInfoBootTime("/proc/uptime",&sys_info);
    // if(status==FALSE){
    //     printf("FALSE\n");
    // }
    // getSysInfoUpTime("/proc/uptime",&sys_info);
    // printf("%s",buf);
    // pid_t pid = 33194;
    // char filename_1[]="/proc/stat";
    // char filename_2[]="/proc/33194/stat";
    // CPU_STAT_INFO *old = getCpuStatInfo(filename_1);
    // PROCESS_CPU_STAT_INFO *oldP = NULL;
    // PROCESS_INFO *proInfo=getProcessInfo(pid,&oldP,NULL,old);
    // if(oldP!=NULL){
    //     printf("pro pid : %d\n",oldP->pid);
    // }else{
    //     printf("NULL\n");
    // }
    // sleep(1);
    // CPU_STAT_INFO *new = getCpuStatInfo(filename_1);
    // // PROCESS_CPU_STAT_INFO *newP =getProcessCpuStatInfo(filename_2);
    // PROCESS_INFO *newProInfo=getProcessInfo(pid,oldP,new,old);
    // printf("pid %d\n",newProInfo->pid);
    // printf("comm %s\n",newProInfo->comm);
    // printf("stat %c\n",newProInfo->task_state);
    // printf("ppid %d\n",newProInfo->ppid);
    // printf("pgid %d\n",newProInfo->pgid);
    // printf("priority %d\n",newProInfo->priority);
    // printf("mem %.2f\n",newProInfo->memery);
    // printf("cpu %.2f\n",newProInfo->cpu);

    // free(old);
    // free(oldP);
    // free(new);
    // free(proInfo);
    // printf(newProInfo);
    // PROCESS_CPU_STAT_INFO *info=getProcessCpuStatInfo(1);
    // if(info!=NULL){
    //     printf("YES\n");
    //     printf("%ld\n",info->cutime);
    // }else{
    //     printf("NO\n");
    //     return -1;
    // }
    // destoryTest(&info);
    // if(info!=NULL){
    //     printf("d YES\n");
    //     printf("%ld\n",info->cutime);
    // }else{
    //     printf("d NO\n");
    // }
    // return 0;
    // PROCESS_INFO *proArray[PID_MAX_NUMBER];   //定义指针数组
    // CPU_STAT_INFO *oldCpuStatInfo;
    // CPU_STAT_INFO *newCpuStatInfo;
    // int i;
    // char cpuPath[]="/proc/stat";

    // printf("init\n");
    // initProcessInfoArray(proArray);           //进程数组初始化
    // printf("get\n");
    // getAllProcessInfo(proArray);              //获取全部进程状态信息
    // printf("get end\n");
    
    // oldCpuStatInfo=getCpuStatInfo(cpuPath);
    
    // sleep(3);
    

    
    // newCpuStatInfo=getCpuStatInfo(cpuPath);
    // printf("update\n");
    // updateAllProcessInfo(proArray,newCpuStatInfo,oldCpuStatInfo);
    // printf("end update\n");
    // printf("start print\n");
    // printInfo(proArray,"res/info.txt");
    // printf("start destory\n");
    // destoryCpuStatInfo(&newCpuStatInfo);
    // destoryCpuStatInfo(&oldCpuStatInfo);
    // destoryProcessArray(proArray);
    // printf("end\n");
    // printf("sizeof(PROCESS_INFO *)=%d\n",sizeof(PROCESS_INFO *));
    showMainWindow(argc,argv);
    return 0;
}