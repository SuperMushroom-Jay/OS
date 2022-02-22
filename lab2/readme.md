#centOS 7 编译内核并增加新的系统调用
---
# 一、编译内核
## 1、下载linux内核
到 [Linux 内核官方网站](https://www.kernel.org/) 下在对应的内核版本
## 2、 解压进入目录
<pre>tar -zxf linux-4.16.1.tar.xz
tar –xvf linux-4.16.1.tar
cd linux-4.16.1/</pre>
## 3、 下载相关文件
<pre>
yum install gcc g++ gdb make //安装gcc g++ gdb make 工具

//以下工具在编译内核时会提醒缺少，可先安装
yum install ncurses-devel 

         

yum install elfutils-libelf-devel

yum install openssl-devel
</pre>
## 4、配置内核
<pre>
make menuconfig     //配置内核，出现配置菜单，直接点save->ok->exit->exit即可

make -j4            //4线程编译内核

make bzImage        //提示有bzImage错误的时候才使用此命令，否则忽略此命令

make modules_install    //安装内核模块
make install         

reboot -n           //重启
</pre>

## 5、选择内核进入linux
---
# 二、新增系统调用
## 1、中断调用表新增自己的系统调用函数
共有 x32 64 common 三种函数，根据自己选择一种，根据其模板写出来即可
<pre>
vi arch/x86/entry/syscalls/syscall_64.tbl
</pre>
eg.
<pre>
333	64		mycopy		sys_mycopy
</pre>
## 2、在头文件中添加新调用的函数
<pre>
vi include/linux/syscalls.h
</pre>
eg.
<pre>
asmlinkage long sys_mycopy(const char __user *source, const char __user *target);
</pre>
## 3、编写自己的系统调用函数
<pre>
vi kernel/sys.c
</pre>
eg.
<pre>
/*new syscall of my copy program*/
SYSCALL_DEFINE2(mycopy, const char __user *, source, const char __user *, target)
{

	int f_source;
	int f_target;
	int  ret = 0;
	char buf[1024];
	mm_segment_t old_fs;
	printk("start copy\n");
	f_source = sys_open(source, O_RDONLY , 0);
	if(f_source==-1){
		printk("open source file failed\n");
		return 1;
	}
	f_target=sys_open(target,O_WRONLY | O_CREAT,0666);
	if(f_target==-1){
		printk("open target file failed\n");
		return 2;
	}
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	//mermony op
	while((ret = sys_read(f_source, buf, 1024))>0){
		printk("transform Bytes: %d\n",ret);
		if(sys_write(f_target, buf, ret)!=ret){
			printk("copy error\n");
			return 3;
		}
	}
	printk("end copy\n");
	//end mermony op
	set_fs(old_fs);
	
	sys_close(f_source);
	sys_close(f_target);
	return 0;
}
</pre>
## 4、再次编译内核，步骤如上述一的的步骤4
---
# 三、测试新增的系统调用函数
编写一个测试程序test.c
<pre>
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
</pre>
然后使用gcc编译
<pre>gcc test.c -o test

//这是一个复制程序,将sourceFile.txt内容拷贝到targetFile.txt中
./test sourceFile.txt targetFile.txt

使用diff命令比较复制结果
diff sourceFile.txt targetFile.txt
若命令行没有其他输出则表示两个文件一致
</pre>
---


