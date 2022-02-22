#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <dirent.h>

//缓冲区大小为1024字节
#define BUF_SIZE 1024 

/**
 * 只做-f与-r指令
 * -f -r -fr -rf
*/
const char parm[4][5]={
   "-f",
   "-r",
   "-rf",
   "-fr"
};

char buf[BUF_SIZE];



int main(int argc,char **argv){
   struct stat s_buf;   //文件描述
   if(argc<3){
      printf("parms less than 3\n");
      exit(-1);
   }
   int i;
   int mode=matchParm(argv[1]);
   switch (mode)
   {
   //-r指令,对于普通文件来说没有意思，只有源文件是目录，目标文件是目录才可发挥作用
   case 1:{
      i=2;
      stat(argv[argc-1],&s_buf);
      if(S_ISDIR(s_buf.st_mode)){
         for(;i<argc-1;i++){
            stat(argv[i],&s_buf);
            //如果是普通文件
            if(S_ISREG(s_buf.st_mode)){
               copyFileToDir(argv[i],argv[argc-1],mode);
               continue;
            }
            if(S_ISDIR(s_buf.st_mode)){
               copyDirToDir(argv[i],argv[argc-1],argv[i],mode);
            }
         }
      }else if(S_ISREG(s_buf.st_mode)){
         if(argc<=4){
            copyFileToFile(argv[2],argv[3],mode);
         }
      }

   }break;
   //-rf与-fr指令同
   case 2:;
   case 3:{

   }break;
   case 0:;
   default:{   //无参数情况
      default_mode:
      if(isFileDir(argv[argc-1])){  //如果目标文件是目录
         i=1;
         if(mode==0){
            i=2;
         }
         for(;i<argc-1;i++){
            char path[128];
            sprintf(path , "%s/%s" , argv[argc-1] , argv[i]);  //目标文件目录
            if(copyFile(argv[i],path,mode)!=0){
               printf("copy error\n");
               exit(-1);
            }
         }
      }
      else{ //如果目标文件不是目录
         if(mode!=0){
            if(argc>3){
               printf("command failed\n");
               exit(-1);
            }else{
              copyFile(argv[1],argv[2], mode);
            }
         }else{
            if(argc>4){
               printf("command failed\n");
               exit(-1);
            }else{
              copyFile(argv[2],argv[3], mode);
            }
         }
      }
   }break;
   }   
   return 0;
}

/**
 * 文件拷贝文件
*/
int copyFile(const char * src, const char *dis, int mode){
   int f_source;  //源文件描述符
   int f_target;  //目标文件描述符
   int ret;
   struct stat s_buf;   //文件描述
   char c;
   //判断文件是否存在
   if(isFileAccess(src)==0){
      printf("%s file not exit\n",src);
      return 0;
   }
   //判断文件是否是目录
   if(isFileDir(src)){
      printf("ignore dir: %s \n",src);
      return 0;
   }
   //源文件以只读模式打开
   f_source=open(src,O_RDONLY);
   //判断目标文件是否存在
   if(isFileAccess(dis)&&mode==4){
      printf("file %s has exit, cover ? (y/n):",dis);
      c=getchar();
      getchar();     //接收回车
      if(c!='y'||c!='Y'){
         return 0;
      }
   }
   //文件不存在时创建文件
   f_target=open(dis,O_WRONLY | O_CREAT,0666);      
   if(f_target==-1){
      printf("file copy failed, reasion:path %s loos some dir\n",dis);
      return -1;
   }
   while ((ret=read(f_source,buf,BUF_SIZE))>0)
   {
      if(write(f_target,buf,ret)!=ret){
         printf("copy progress error\n");
         return -2;
      }
   }
   return 0;
}

//目录拷贝
int copyDir(const char * src, const char *dis){

   return 0;
}

/**
 * 参数匹配
 * 
*/
int matchParm(char *p){
   int i=0;
   for(i=0;i<4;i++){
      if(strcmp(parm[i],p)==0){
         return i;
      }
   }
   return -1;
}

/**
 *文件是否存在
*/
int isFileAccess(const char* filename){
   if(access(filename,F_OK)==0) return 1;
   else return 0;
}

/**
 * 文件是否为目录
*/
int isFileDir(const char *filename){
   struct stat s_buf;   //文件描述
   stat(filename,&s_buf);
   return S_ISDIR(s_buf.st_mode);
}

/**
 * 将文件复制到文件中
 * 
*/
int copyFileToFile(const char *src,const char *dist,int mode){
   int f_source;  //源文件描述符
   int f_target;  //目标文件描述符
   int ret;
   struct stat s_buf;   //文件描述
   char c;
   //源文件以只读模式打开
   f_source=open(src,O_RDONLY);
   //判断目标文件是否存在
   if(isFileAccess(dist)&&mode==4){
      printf("The file %s already exists. Do you want to overwrite it ? (y/n):",dist);
      c=getchar();
      getchar();     //接收回车
      if(c!='y'||c!='Y'){
         return 0;
      }
   }
   //文件不存在时创建文件
   f_target=open(dist,O_WRONLY | O_CREAT,0666);      
   if(f_target==-1){
      printf("file copy failed, reasion:path %s loos some dir\n",dist);
      return -1;
   }
   while ((ret=read(f_source,buf,BUF_SIZE))>0)
   {
      if(write(f_target,buf,ret)!=ret){
         printf("copy progress error\n");
         return -2;
      }
   }
   printf("file: %s ===> file: %s \n",src,dist);
   return 0;
}
/**
 * 将文件复制到目录中
 * 
*/
int copyFileToDir(const char *src,const char *dist,int mode){
   char path[128];
   sprintf(path , "%s/%s" , dist , src);  //目标文件目录
   printf("file: %s ===> dir: %s \n",src,dist);
   copyFileToFile(src,path,mode);
}
/**
 * 将目录复制到目录中
 * 
*/
int copyDirToDir(const char *src,const char *dist,const char *filename,int mode){
   int ret;
   char distPath[128];
   char tempDistPath[128];
   char temSrcPath[128];
   struct stat  s_buf ; 
	DIR *dir = NULL ; 
	struct dirent *entry = NULL ;
   
   sprintf(distPath,"%s/%s",dist,filename);   //目录路径
   ret = stat(distPath , &s_buf);             //判断目录是否存在
   if(ret==-1){         //目标目录不存在
      ret = mkdir(distPath,0777);   //创建目标目录
      dir=opendir(src); //打开源目录
      if(dir==NULL){
         printf("open source dir: %s falied\n",src);
         return -1;
      }
      //递归复制文件
      while(1){
         //读取文件夹
         entry = readdir(dir);
			if(entry==NULL){
            printf("copy end\n");
            break;
         }
         //跳过 . 和 ..
			if((strcmp(entry->d_name , ".")==0)||(strcmp(entry->d_name , "..")==0))
				continue ; 
				
			sprintf(tempDistPath , "%s/%s" , distPath , entry->d_name);   //目标目录或目标文件路径
			sprintf(temSrcPath , "%s/%s" , src , entry->d_name);   //源文件或源目录路径
			stat(temSrcPath , &s_buf);
         //是否是一个常规文件
			if(S_ISREG(s_buf.st_mode)){
            printf("copy file\n");
				copyFileToFile(temSrcPath,tempDistPath,mode);
			}
         //是否是一个文件夹
			else if(S_ISDIR(s_buf.st_mode)){
            printf("dir: %s ===> dir: %s \n",tempDistPath,distPath);
				copyDirToDir(tempDistPath , distPath , entry->d_name, mode);			
			}
      }
   }
   return 0;
}	
 

