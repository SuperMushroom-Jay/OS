#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 1024

 int main(void)
{
	int fd;
	char buf[BUF_SIZE];
	char get[BUF_SIZE];
    memset(get, 0, sizeof(get)); 
    memset(buf, 0, sizeof(buf)); 
    printf("please enter a string you want input to character_driver:\n"); 
    gets(get);

	fd = open("/dev/character_driver", O_RDWR); //打开设备    

    //printf("fd=%d\n",fd);

	if (fd > 0) { 
        read(fd, &buf, sizeof(buf)); 
        printf("The message in character_driver now is: %s\n", buf); 
 
        //将输入写入设备 
        write(fd, &get, sizeof(get)); 
        //读出设备的信息并打印 
        read(fd, &buf, sizeof(buf)); 
        printf("The message in character_driver changed to: %s\n", buf); 
    }  
    else { 
        printf("character_driver open failed\n"); 
        return -1; 
    }    
	close(fd);//释放设备
    return 0;
}
