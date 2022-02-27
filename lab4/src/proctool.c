#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include"proctool.h"
#include"stringtool.h"

/**
 * @brief 执行命令cmd并获取shell上的输出信息
 * 
 * @param cmd 命令
 * @param data shell返回的数据写入data中 
 * @param dataLen 数据容量
 * @return char* 返回命令行数据
 */
boolean readCMD(const char* cmd,char *data,int dataLen){
    FILE *fp;
    fp=popen(cmd,"r");
    if(fp==NULL){
        return FALSE;
    }
    memset(data,'\0',dataLen);
    fread(data,sizeof(char),dataLen,fp);
    pclose(fp);
    //printf("%s\n",data);
    return TRUE;
}

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
