#include"type.h"
#include"process.h"
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * @brief 获取进程的信息，获取成功返回地址，否则返回NULL
 * 
 * @param pid 进程号
 * @return PROCESS_INFO* 
 */
PROCESS_INFO * getPidInfo(pid_t pid){
    char path[64];
    sprintf(path,"/proc/%d/stat",pid);  //获取路径信息
    FILE *fp;
    fp=fopen(path,"r");
    if(fp==NULL){
        return NULL;
    }
    PROCESS_INFO *info=(PROCESS_INFO *)malloc(sizeof(PROCESS_INFO));
    fscanf(fp,"%d%s%c%d%d",&info->pid,info->comm,&info->task_state,&info->ppid,&info->pgid);
    
}