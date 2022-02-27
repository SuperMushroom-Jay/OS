#include"show.h"
#include<stdio.h>

int main(int argc,char **argv){
    DATA data;
    char title[]="add";
    data.mode='+';
    data.t_pid=0;
    data.title=title;
    showWindow(argc,argv,&data);
    printf("add end\n");
    return 0;
}
