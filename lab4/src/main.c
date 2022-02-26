#include"stringtool.h"
#include<stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(){
    int status = 0;
    status = string_match("helohell","^hell");
    printf("status=%d\n",status);
    return 0;
}