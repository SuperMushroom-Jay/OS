/*new syscall of my copy*/
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