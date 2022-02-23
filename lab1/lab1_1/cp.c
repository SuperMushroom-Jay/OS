#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <dirent.h>

#include <libgen.h>
//char *dirname(char *path);     //  [/data/a, /data] [/data/, /] [data, .] 
//char *basename(char *path);    //  [/data/a, a] [/data/, data] [data, data]


//缓冲区大小为1024字节
#define BUF_SIZE 1024 


/*********************全局参数*******************/
/**
 * 只做-f与-r指令
 * -f -r -v
*/
//命令 0 1 2 3
const char parm[5]="frvi";

char buf[BUF_SIZE];
int commandExist=0;  //参数是否存在
int f=0;
int r=0;
int v=0;
int i=0;

/*********************函数声明*******************/
int copyFileToFile(const char *src,const char *dist);
int copyFileToDir(const char *src,const char *dist);
int copyDirToDir(const char *src,const char *dist,const char *filename);
int isCommand(const char *command);
int isCommandParm(char *command,char parm);
int fileType(const char * filename,struct stat *s_buf);
void printFileTypeError(const char * filename, int errorType);
void dealParm(char *parm);

/*********************函数*******************/
int main(int argc,char **argv)
{
   if(argc<3){
      printFileTypeError("",-1);
      exit(-1);
   }
   struct stat s_buf;   //文件描述
   char *dist=argv[argc-1];
   //f,r,v代表相应参数是否存在
   int type;
   int index=1;
   //存在参数
   if((commandExist=isCommand(argv[1]))==1)
   {
      //获取对应参数是否存在
      f = isCommandParm(argv[1],'f');
      r = isCommandParm(argv[1],'r');
      v = isCommandParm(argv[1],'v');
      i = isCommandParm(argv[1],'i');
      //printf("f=%d  r=%d  v=%d  i=%d\n",f,r,v,i);
      //文件参数从argv[2]开始
      index=2;
   }
   type=fileType(dist,&s_buf);
   switch (type)
   {
   case -1://目标文件或目录不存在
         /**
          * 一、单纯的一个文件的复制
          * 二、其他情况都不符合
          * 
          *  * 一、单纯的单文件复制，参数只能是4个
             * 二、单纯的单目录复制，参数只能是4个
             * 三、其他情况都不符合
          * */
   case 0://目标是文件
      {
         if(argc-commandExist-1==2){
            int tempType=fileType(argv[index],&s_buf);  //源文件类型
            switch (tempType)
            {
            case -1://源文件不存在
               printFileTypeError(argv[index],0);   //源文件不存在
               break;
            case 0://源文件是普通文件
               copyFileToFile(argv[index],dist);
               break;
            case 1://源文件是目录
               if(r==1&&type==-1){
                  dealParm(argv[index]);
                  copyDirToDir(argv[index],dist,argv[index]);
               }
               else 
                  printFileTypeError(argv[index],1);   //忽略目录
               break;
            default:
               break;
            }
         }else
            printFileTypeError(dist,3);   //目标不是目录
      }break;

   case 1:{ //目标是目录
         dealParm(dist);
         while (index<argc-1){
            type=fileType(argv[index],&s_buf);  //源文件类型
            switch (type){
            case -1://源文件不存在
               printFileTypeError(argv[index],0);   //源文件不存在
               break;
            case 0://源文件是普通文件
               copyFileToDir(argv[index],dist);
               break;
            case 1://源文件是目录
               if(r==1){
                  dealParm(argv[index]);
                  copyDirToDir(argv[index],dist,argv[index]);
               }
               else
                  printFileTypeError(argv[index],1);   //忽略目录
               break;
            default:
               break;
            }
            index++;
         }
      }break;
   default:
      break;
   }
   return 0;
}

/**
 * 将文件复制到文件中
 * 
*/
int copyFileToFile(const char *src,const char *dist){
   int f_source;  //源文件描述符
   int f_target;  //目标文件描述符
   int ret;
   struct stat s_buf;   //文件描述
   char c;
   //源文件以只读模式打开
   f_source=open(src,O_RDONLY);
   //判断目标文件是否存在
   if(fileType(dist,&s_buf)==0&&f==0&&i==1){
      printFileTypeError(dist,4);   //目标文件已经存在，是否覆盖？
      c=getchar();
      getchar();     //接收回车
      if(c!='y'||c!='Y'){
         return 0;
      }
   }
   //文件不存在时创建文件
   f_target=open(dist,O_WRONLY | O_CREAT,0666);      
   if(f_target==-1){
      printFileTypeError(dist,0);
      return -1;
   }
   while ((ret=read(f_source,buf,BUF_SIZE))>0)
   {
      if(write(f_target,buf,ret)!=ret){
         printFileTypeError(dist,5);   //复制写入过程中发生错误
         return -2;
      }
   }
   if(v==1)
      printf("\"%s\" ===> \"%s\" \n",src,dist);
   return 0;
}
/**
 * 将文件复制到目录中
 * 
*/
int copyFileToDir(const char *src,const char *dist){
   char path[128];
   sprintf(path , "%s/%s" , dist , basename(src));  //目标文件目录
   copyFileToFile(src,path);
}
/**
 * 将目录复制到目录中
 * 
*/
int copyDirToDir(const char *src,const char *dist,const char *filename){

   //printf("--------------------%s------------------>\n",src);

   int ret;
   char distPath[128];
   char tempDistPath[128];
   char temSrcPath[128];
   struct stat  s_buf ; 
	DIR *dir = NULL ; 
	struct dirent *entry = NULL ;
   
   //首先判断这个目录是否存在
   ret = stat(dist, &s_buf);
   if(ret==-1){
      sprintf(distPath,"%s",dist);
   }else{
      sprintf(distPath,"%s/%s",dist,filename);   //目录路径    
   }
   ret = stat(distPath , &s_buf);    //再判断加了filename的目录或者文件是否存在
   //printf("distPath: %s\n",distPath);   
      if(ret==-1&&mkdir(distPath,0777)==-1)  //创建目标目录
         return -1;
      dir=opendir(src); //打开源目录
      if(dir==NULL){
         printFileTypeError(src,6); //目录打开失败
         return -1;
      }
      if(v==1)
         printf("\"%s/\" ===> \"%s/\" \n",src,distPath);
      //递归复制文件
      while(1){
         //读取文件夹
         entry = readdir(dir);
			if(entry==NULL){
            //某个文件夹以及读完
            //printf("%s open failed\n",src);
            break;
         }
         
         //跳过 . 和 ..
			if((strcmp(entry->d_name , ".")==0)||(strcmp(entry->d_name , "..")==0))
				continue ; 

         //printf("now file: %s\n",entry->d_name);   
				
			sprintf(tempDistPath , "%s/%s" , distPath , entry->d_name);   //目标目录或目标文件路径
			sprintf(temSrcPath , "%s/%s" , src , entry->d_name);   //源文件或源目录路径
			stat(temSrcPath , &s_buf);
         //printf("next src file: %s\n",temSrcPath);
         //printf("next dist file: %s\n",tempDistPath);
         //是否是一个常规文件
			if(S_ISREG(s_buf.st_mode)){
            //printf("copy file\n");
				copyFileToFile(temSrcPath,tempDistPath);
			}
         //是否是一个文件夹
			else if(S_ISDIR(s_buf.st_mode)){
            //printf("copy dir\n");
				copyDirToDir(temSrcPath , distPath , entry->d_name);			
			}
      }

   //printf("======================%s=======================>\n",src);
   return 0;
}	
/**
 * 第二个参数是否是参数
 * */ 
int isCommand(const char *command){
   if(command[0]=='-') return 1;
   else return 0;
}

/**
 * 是否含有命令参数
*/
int isCommandParm(char *command,char parm){
   int i;
   for(i=0;i<strlen(command);i++){
      if(command[i]==parm)
         return 1;
   }
   return 0;
}

/**
 * 是否是普通文件
*/
int fileType(const char * filename,struct stat *s_buf){
   if(stat(filename,s_buf)==-1){
      return -1;  //文件不存在
   }
   if(S_ISREG(s_buf->st_mode)){
      return 0;   //文件
   }
   if(S_ISDIR(s_buf->st_mode)){
      return 1;   //目录
   }
}
/**
 * 是否是普通文件
*/
void printFileTypeError(const char * filename, int errorType){
   switch (errorType){
   case -1:
      printf("the necessary files are missing after cp\n");
      break;
   case 0:
      printf("\"%s\" : there is no such file or directory\n",filename);
      break;
   case 1:
      printf("\"%s\" : ignore the directory\n",filename);
      break;
   case 2:
      printf("\"%s\" : the file does not exist\n",filename);
      break;
   case 3:
      printf("\"%s\" : the destination is not a directory\n",filename);
      break;
   case 4:
      printf("\"%s\" : cover the file?(y/n):",filename);
      break;
   case 5:
      printf("\"%s\" : an error occurred during writing\n",filename);
      break;
   case 6:
      printf("\"%s\" : directory open failed\n",filename);
      break;
   default:
      printf("abnormal error\n");
      break;
   }
}

void dealParm(char *parm){
   if(parm[strlen(parm)-1]=='/')
      parm[strlen(parm)-1]='\0';
}

