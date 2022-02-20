#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//缓冲区大小为512字节
#define BUF_SIZE 512 

int main(int argc,char **argv){
   int f_source;  //源文件描述符
   int f_target;  //目标文件描述符
   int ret;
   struct stat s_buf;   //文件描述
   char c;
   char buf[BUF_SIZE];
   if(argc!=3){
      printf("参数为3个\n");
      exit(-1);
   }
   //源文件存在
   if(access(argv[1],F_OK) == -1){
      printf("%s 文件不存在\n",argv[1]);
      exit(-3);
   }else{
      stat(argv[1],&s_buf);
      if(S_ISDIR(s_buf.st_mode)){
         printf("\"%s\" 文件是目录\n",argv[1]);
         exit(-4);
      }
   }
   //目标文件是否存在 且 可写 且 不是目录
   if(access(argv[2],F_OK) != -1){
      stat(argv[2],&s_buf);
      if(S_ISDIR(s_buf.st_mode)){
         printf("\"%s\" 文件是目录\n",argv[2]);
         exit(-4);
      }
      printf("是否覆盖(y/n):");
      c=getchar();
      if(c=='n'||c=='N'){
         return 0;
      }
      
   }
   //源文件以只读模式打开
   f_source=open(argv[1],O_RDONLY);
   f_target=open(argv[2],O_WRONLY | O_CREAT,0666); //文件不存在时创建文件
   if(f_source !=-1 || f_target !=-1){
      //printf("源文件打开成功\n");
   }else{
      //printf("源文件打开失败\n");
      exit(-5);
   }
   if(f_target != -1){
      //printf("目标文件打开成功\n");
   }else{
      printf("没有 \"%s\" 文件或目录\n",argv[2]);
      exit(-6);
   }
   do{
      ret=read(f_source,buf,BUF_SIZE);
      write(f_target,buf,ret);
   }while(ret>0);
   close(argv[1]);
   close(argv[2]);
   return 0;
}


