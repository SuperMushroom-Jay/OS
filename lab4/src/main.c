#include"stringtool.h"
#include"proctool.h"
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(){
    CPU_INFO *cpu_info=NULL;
    cpu_info=get_cpu_info("/proc/cpuinfo");
    printf("status=%d\n",string_match("ababbabc","^abb"));
    if(cpu_info!=NULL){
        printf("%s\n",cpu_info->vendor_id);
        printf("%s\n",cpu_info->model_name);
        printf("%s\n",cpu_info->cpu_mhz);
        printf("%s\n",cpu_info->cache_size);
    }else{
        printf("error\n");
    }
    free(cpu_info); //释放内存
    return 0;
}