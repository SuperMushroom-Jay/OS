#include"show.h"
#include<stdio.h>

int main(int argc,char **argv){
    DATA data;
    char title[]="sub";
    data.t_pid=0;
    data.mode='-';
    data.title=title;
    showWindow(argc,argv,&data);
    printf("sub end\n");
    return 0;
}