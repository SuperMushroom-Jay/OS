#include<linux/kernel.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>

int main(int argc ,char *argv[])
{
    long int ret=syscall(333,argv[1],argv[2]);
    printf("return %ld\n",ret);
    return 0;
}