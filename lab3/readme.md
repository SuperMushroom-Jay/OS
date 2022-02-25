# centOS 7 采用模块的方法添加设备--字符设备驱动程序
---
## 一、设备驱动程序的介绍
- 对设备初始化和释放
- 把数据从内核传送到硬件和从硬件读取数据
- 读取应用程序传送给设备文件的数据和回送应用程序请求的数据
- 检测和处理设备出现的错误.
## 二、编写设备驱动程序的源代码
在设备驱动程序中有一个非常重要的结构file_operations，该结构的每个域都对应着一个系统调用。
<pre>
struct file_operations { 
    int (*seek) (struct inode * ，struct file *， off_t ，int);  
    int (*read) (struct inode * ，struct file *， char ，int);  
    int (*write) (struct inode * ，struct file *， off_t ，int);  
    int (*readdir) (struct inode * ，struct file *， struct dirent * ,int);  
    int (*select) (struct inode * ，struct file *， int ，select_table *);  
    int (*ioctl) (struct inode * ，struct file *， unsined int ，unsigned long);  
    int (*mmap) (struct inode * ，struct file *， struct vm_area_struct *);  
    int (*open) (struct inode * ，struct file *);  
    int (*release) (struct inode * ，struct file *);  
    int (*fsync) (struct inode * ，struct file *);  
    int (*fasync) (struct inode * ，struct file *，int);  
    int (*check_media_change) (struct inode * ，struct file *);  
    int (*revalidate) (dev_t dev);  
} 
</pre>

## 三、编写设备驱动程序的子函数并填写file_operations的各个域
- 例如：定义字符驱动程序的子函数如下
<pre>
static int character_open(struct inode *inode, struct file *filp);      //打开函数  
static int character_release(struct inode *inode, struct file *filp);   //释放函数
static int character_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);        //读函数
static int character_wirte(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);      //写函数
</pre>
- 填充file_operations
<pre>
static struct file_operations character_driver_fops =
{   
    .owner = THIS_MODULE,
    .open = character_open,
    .read = character_read,
    .write = character_wirte,
    .release = character_release,
};
</pre>
- 编写子函数，具体代码参考同目录下的character_driver.c文件
## 四、编写驱动设备程序的初始化函数和卸载函数
- 对于可卸载的内核模块来说，至少包括如下两个模块
<pre>
/*device sub func*/
编写子函数
/*device sub func*/

static int  character_driver_init(void)  //注册设备，获取设备主设备好
{
    /*code/*
}
static void  character_driver_exit(void) //注销设备
{
    /*code*/
}

//在末尾调用上诉两个函数
module_init(character_driver_init);
module_exit(character_driver_exit);

MODULE_LICENSE("GPL");
</pre>
- 其中 character_driver_init 用于注册设备，获取设备的主设备号
- 其中 character_driver_exit 用于注销设备
## 五、编写自己设备驱动程序源代码
- 代码请参考character_driver.c文件
## 六、编写Makefile文件编译设备驱动程序代码
- 示例如下
<pre>
ifneq ($(KERNELRELEASE),)    
    
obj-m := character_driver.o        
else
PWD :=$(shell pwd)
KVER :=$(shell uname -r)
KDIR :=/lib/modules/$(KVER)/build
all:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
#	rm -f *.cmd *.o *.mod *.ko
	rm -rf .*.cmd *.o *.mod.c *.ko .tmp_versions
#	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif
</pre>
## 七、编译设备驱动代码
- 需要获取root权限，输入命令su root获取root权限
- 找到并进入你编写的 “设备驱动程序代码” 所在的文件夹
- 依次执行以下命令
<pre>
make    //执行此命令会根据Makefile内容编译

insmod character_driver.ko  //安装character_driver驱动模块文件

lsmod   //列出已经安装的设备驱动程序

cat /proc/devices           //在列出来的信息中是否能找到 "240 character_driver" 
//其中240是主设备号，后面是设备名

mknod /dev/character_driver c 240 0  //在"/dev/"目录下创建character_driver字符设备文件
//参数1 /dev/character_driver : 新建驱动设备文件的地址和名字
//参数2 c : 创建的是字符驱动设备文件
//参数3 240 : 主设备号
//参数4 0 : 从设备号

find /dev/ -name character_driver  //"/dev/"目录下查找character_driver文件
//执行上述命令后可以在命令行中显示出查找到character_driver文件

chmod 666 /dev/character_driver    //修改文件权限
</pre>
- 若要卸载设备驱动程序执行以下命令
<pre>
rmmod /dev/character_driver     //卸载设备驱动文件

rm /dev/character_driver        //删除设备驱动文件
</pre>
## 八、编写测试代码
- 测试代码为同目录下的test.c文件