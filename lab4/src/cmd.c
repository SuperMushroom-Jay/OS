#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include"cmd.h"
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




