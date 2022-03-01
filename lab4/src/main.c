#include"include.h"
// #include"cpu.h"
// #include"type.h"
// #include<stdio.h>

int main(){
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
    // char filename_1[]="/proc/stat";
    // char filename_2[]="/proc/19150/stat";
    // CPU_STAT_INFO *old = getCpuStatInfo(filename_1);
    // PROCESS_CPU_STAT_INFO *oldP =getProcessCpuStatInfo(filename_2);
    // sleep(10);
    // CPU_STAT_INFO *new = getCpuStatInfo(filename_1);
    // PROCESS_CPU_STAT_INFO *newP =getProcessCpuStatInfo(filename_2);
    // printf("cpu precent = %.2f\n",getCpuPercent(new,old));
    // printf("process precent = %.2f\n",getProcessCpuPercent(newP,oldP,new,old));
    // free(old);
    // free(oldP);
    // free(new);
    // free(newP);
    const char filename[]="/proc/meminfo";
    MEM_INFO *info;
    info = getMemInfo(filename);
    float precent = getMemPrecent(info);
    float pmem = getProcessMem(27685);
    printf("mem precent = %.2f\n",precent);
    printf("firefox mem = %.2f\n",pmem);
    return 0;
}