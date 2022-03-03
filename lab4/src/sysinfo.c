#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include"sysinfo.h"
#include"stringtool.h"

/**
 * @brief 获取文件中的第一行数据
 * 
 * @param filename 文件
 * @param data 将文件中第一行的数据写入data中
 * @return boolean 写入成功返回TRUE，否则返回FALSE
 */
boolean getFileFristLine(const char *filename,char *data){
    FILE *fp;
    fp=fopen(filename,"r");
    if(fp==NULL){
        return FALSE;
    }
    char buf[SYS_BUF_SIZE];
    if(fgets(buf,SYS_BUF_SIZE,fp)==NULL){
        return FALSE;
    }else{
        memcpy(data,buf,SYS_BUF_SIZE);
    }
    fclose(fp);
    return TRUE;
}

/**
 * @brief 获取系统的主机名
 * 
 * @param filename 主机名存储地址 /proc/sys/kernel/hostname
 * @param sysInfo 存储系统信息的结构体
 * @return boolean 获取成功返回TRUE,否则返回FALSE
 */
boolean getSysInfoHostName(const char *filename,SYS_INFO *sysInfo){
    return getFileFristLine(filename,sysInfo->hostName);
}


/**
 * @brief 获取系统开机时间
 * 
 * @param filename 系统开机时间 /proc/uptime
 * @param sysInfo 存储系统信息的结构体
 * @return boolean 获取成功返回TRUE,否则返回FALSE
 */
boolean getSysInfoBootTime(const char *filename,SYS_INFO *sysInfo){
    FILE *fp;
    time_t uptime;
    time_t cur_time;
    time_t boot_time;
    fp=fopen(filename,"r");
    if(fp==NULL){
        return FALSE;
    }
    if(fscanf(fp,"%ld",&uptime)==0){
        return FALSE;
    }
    fclose(fp);
    time(&cur_time);
    boot_time =cur_time-uptime;
    memcpy(sysInfo->bootTime,ctime(&boot_time),SYS_BUF_SIZE);
    //printf("%s\n",sysInfo->bootTime);
    return TRUE;
}

/**
 * @brief 获取系统开机持续的时间
 * 
 * @param filename /proc/uptime 文件
 * @param sysInfo 存储系统信息的结构体
 * @return boolean 获取成功返回TRUE,否则返回FALSE
 */
boolean getSysInfoUpTime(const char *filename,SYS_INFO *sysInfo){
    time_t uptime;
    FILE *fp;
    fp=fopen(filename,"r");
    if(fp==NULL){
        printf("open %s error\n",filename);
        return FALSE;
    }
    if(fscanf(fp,"%ld",&uptime)==0){
        printf("read %s error\n",filename);
        return FALSE;
    }
    
    unsigned long day = uptime/(60*60*24);
    uptime%=(60*60*24);
    unsigned long hour = uptime/(60*60);
    uptime%=(60*60);
    unsigned long minute = uptime/(60);
    unsigned long second = uptime%(60);
    sprintf(sysInfo->upTime,"%ld days---%ld:%ld:%ld\n",day,hour,minute,second);
    fclose(fp);
    return TRUE;
}

/**
 * @brief 获取系统的类型
 * 
 * @param filename 文件名路径 /proc/sys/kernel/ostype 
 * @param sysInfo 存储系统信息的结构体
 * @return boolean 获取成功返回TRUE,否则返回FALSE
 */
boolean getSysInfoOsType(const char *filename,SYS_INFO *sysInfo){
    return getFileFristLine(filename,sysInfo->osType);
}

/**
 * @brief 获取系统内核版本
 * 
 * @param filename 文件名路径 /proc/sys/kernel/osrelease
 * @param sysInfo 存储系统信息的结构体
 * @return boolean 获取成功返回TRUE,否则返回FALSE
 */
boolean getSysInfoOsRelease(const char *filename,SYS_INFO *sysInfo){
    return getFileFristLine(filename,sysInfo->osRelease);
}

/**
 * @brief 获取系统信息，若获取成功返回系统信息结构体指针，否则返回NULL
 * 
 * @return SYS_INFO* 
 */
SYS_INFO* getSysInfo(){
    boolean status;
    SYS_INFO* sys_info=(SYS_INFO*)malloc(sizeof(SYS_INFO));
    status = getSysInfoHostName("/proc/sys/kernel/hostname",sys_info);
    if(status==FALSE){
        free(sys_info);
        return NULL;
    }
    status = getSysInfoBootTime("/proc/uptime",sys_info);
    if(status==FALSE){
        free(sys_info);
        return NULL;
    }
    status = getSysInfoUpTime("/proc/uptime",sys_info);
    if(status==FALSE){
        free(sys_info);
        return NULL;
    }
    status = getSysInfoOsType("/proc/sys/kernel/ostype",sys_info);
    if(status==FALSE){
        free(sys_info);
        return NULL;
    }
    status = getSysInfoOsRelease("/proc/sys/kernel/osrelease",sys_info);
    if(status==FALSE){
        free(sys_info);
        return NULL;
    }
    return sys_info; 
}

/**
 * @brief 销毁SYS_INFO * 申请的空间
 * 
 * @param info 
 */
void destorySysInfo(SYS_INFO **info){
    if(*info!=NULL){
        free(*info);
        *info=NULL;
    }
}