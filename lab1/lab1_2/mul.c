#include"show.h"
#include<stdio.h>

int main(int argc,char **argv){
    DATA data;
    char title[]="mul";
    data.t_pid=0;
    data.mode='x';
    data.title=title;
    showWindow(argc,argv,&data);
    printf("mul end\n");
    return 0;
}