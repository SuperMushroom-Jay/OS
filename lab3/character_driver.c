#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

/*DEFINE*/
#define CHARACTER_MAJOR 240   //驱动设备号
#define BUF_SIZE 1024   //缓冲区大小
#define TRUE 0
#define FALSE -1

/*FUNC STATEMENT*/
static int character_open(struct inode *inode, struct file *filp);      //打开函数  
static int character_release(struct inode *inode, struct file *filp);   //释放函数
static int character_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);        //读函数
static int character_wirte(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);      //写函数
static int  character_driver_init(void);  //初始化函数
static void  character_driver_exit(void); //卸载函数

/*GLOBAL VARS*/
static char *character_buf;  //缓冲区
static int opne_pid_mutex = 0;     //设备范文互斥信号量
//填充file_operations结构相关入口
static struct file_operations character_driver_fops =
{   
    .owner = THIS_MODULE,
    .open = character_open,
    .read = character_read,
    .write = character_wirte,
    .release = character_release,
};

/*DEFINE FUNC*/
/**
 * @brief 打开函数
 * 
 * @param inode 
 * @param filp 
 * @return int 
 */
static int character_open(struct inode *inode, struct file *filp)      //打开函数  
{
    printk("\n===> Main device : %d <===\n",MAJOR(inode->i_rdev));
    printk("===> Slave device : %d <===\n",MINOR(inode->i_rdev));
    if(opne_pid_mutex == 0){
        opne_pid_mutex = 1;
        try_module_get(THIS_MODULE);
        return TRUE;
    }else{
        //发出警告
        printk(KERN_ALERT"===> another process open the character driver <===\n");
        return FALSE;
    }
}

/**
 * @brief 
 * 
 * @param inode 
 * @param filp 
 * @return int 
 */
static int character_release(struct inode *inode, struct file *filp)   //释放函数
{   
    opne_pid_mutex=0;   //释放设备
    printk("\n===> the character released <===\n");
    module_put(THIS_MODULE);
    return TRUE;
}

/**
 * @brief 
 * 
 * @param filp 
 * @param buf 
 * @param count 
 * @param f_pos 
 * @return int 
 */
static int character_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)        //读函数
{
    if(copy_to_user(buf,character_buf,count)){  //读缓冲
        return -1;
    }
    return count;
}

/**
 * @brief 
 * 
 * @param filp 
 * @param buf 
 * @param count 
 * @param f_pos 
 * @return int 
 */
static int character_wirte(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)      //写函数
{
    if(copy_from_user(character_buf,buf,count)){    //写缓冲
        return -1;
    }
    return count;
}

/**
 * @brief 开辟缓冲区与注册设备
 * 
 * @return int 
 */
static int  character_driver_init(void)  //初始化函数
{
    int ret;
    character_buf=(char *)kmalloc(BUF_SIZE,GFP_KERNEL);
    //向系统的字符登记表登记一个字符设备
    ret = register_chrdev(CHARACTER_MAJOR,"character_driver",&character_driver_fops);
    if(ret<0){
        printk(KERN_WARNING"\n===> the character_driver register failed <===\n");
        return FALSE;
    }else{
        printk("\n===> the character_driver register success <===\n");
        return TRUE;
    }
}

/**
 * @brief 注销设备和释放缓冲区
 * 
 */
static void  character_driver_exit(void) //卸载函数
{
    printk(KERN_ALERT"\n===> the character_driver start unregister ... <===\n");
    kfree(character_buf);               //释放缓冲区
    unregister_chrdev(CHARACTER_MAJOR,"mydriver");  //注销设备 
    printk("\n===> unregister success! <===\n");
}

/*DEFINE MODULE*/
module_init(character_driver_init);
module_exit(character_driver_exit);

MODULE_LICENSE("GPL");

