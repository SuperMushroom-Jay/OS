#include<stdio.h>
#include<sys/types.h>
#include <unistd.h>
#include <wait.h>

#define PROCESS_NUMBER 3

int main(int argc,char **argv){
    pid_t pid[PROCESS_NUMBER];  //父进程记录子进程的pid
    int local_pid;              //本进程的pid
    int i=0;
    while (i<PROCESS_NUMBER)
    {
        local_pid=fork();
        if(local_pid==0){
            break;
        }else if(local_pid>0){
            pid[i]=local_pid;
        }else{
            printf("process %d fork failed\n",i+1);
        }
        i++;
    }
    switch (i)
    {
    case 0:
        execv("./add.out",argv);
        break;
    case 1:
        execv("./sub.out",argv);
        break;
    case 2:
        execv("./mul.out",argv);
        break;
    default:{
        //等待三个子进程退出
        wait(NULL);
        wait(NULL);
        wait(NULL);
        printf("main end\n");
        }break;
    }
    return 0;
}
