#include"stringtool.h"
#include"proctool.h"
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(){
    SYS_INFO *sys_info=getSysInfo();
    if(sys_info!=NULL){
        printf("%s\n",sys_info->hostName);
        printf("%s\n",sys_info->bootTime);
        printf("%s\n",sys_info->upTime);
        printf("%s\n",sys_info->osType);
        printf("%s\n",sys_info->osRelease);
    }else{
        printf("FALSE");
    }
    // char buf[1024];
    // readCMD("date -d \"$(awk -F. '{print $1}' /proc/uptime) second ago\" +\"%Y-%m-%d %H:%M:%S\"",buf,1024);
    // boolean status = getSysInfoBootTime("/proc/uptime",&sys_info);
    // if(status==FALSE){
    //     printf("FALSE\n");
    // }
    // getSysInfoUpTime("/proc/uptime",&sys_info);
    // printf("%s",buf);
    return 0;
}