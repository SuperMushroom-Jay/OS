#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#define MY_MAJOR 240

char *s1;
char *s2;
int zcydriver_open(struct inode *inode, struct file *filp)
{
    printk("...zcydriver_open\n");
    return 0;
}

ssize_t zcydriver_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{


	sprintf(s2,"%s",s1);
	if(count<12) {
		if(!copy_to_user(buf,s2,count))
		{
			return 0;
		}
	}
	else
	{
		if(!copy_to_user(buf,s2,strlen(s2)))
		{
			return 0;
		}
	}
	return -1;
}

ssize_t zcydriver_write(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    	if (count < 0)
                return -EINVAL;
        if (s1 == NULL)
                return -ENOMEM;
        if (copy_from_user(s1, buf, count+1))
                return -EFAULT;
        return count;
}

int zcydriver_release(struct inode *inode, struct file *filp)
{
    printk("...zcydriver_release\n");
    return 0;
}

struct file_operations zcydriver_fops = {
    .owner = THIS_MODULE,
    .open = zcydriver_open,
    .read = zcydriver_read,
    .write = zcydriver_write,
    .release = zcydriver_release,
};

static int zcydriver_init(void)
{
    int rc;
    s1 = (char *)kmalloc(32, GFP_KERNEL);
    s2=(char *)kmalloc(64,GFP_KERNEL);
    printk("...Test zcydriver dev\n");
    rc = register_chrdev(MY_MAJOR, "zcydriver", &zcydriver_fops);
    if(rc < 0)
    {
        printk("...register %s  dev error\n", "zcydriver");
        return -1;
    }
    printk("...register zcydriver dev OK\n");
    return 0;
}

static void zcydriver_exit(void)
{
    unregister_chrdev(MY_MAJOR, "zcydriver");
    printk("...Good Bye!\n");
}

MODULE_LICENSE("GPL");
module_init(zcydriver_init);
module_exit(zcydriver_exit);

