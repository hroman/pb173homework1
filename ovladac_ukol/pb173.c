#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

int info = 0; 

int my_open ( struct inode *inode , struct file * filp ) {
	printk(KERN_INFO "modul otevren\n");		
	return 0;
}

ssize_t my_write ( struct file * filp , const char __user *buf , size_t
	count , loff_t * offp ) 
{	
	char mem[100];
	if (count > 100)
		return -EFAULT;
	if (copy_from_user(mem, buf, count))
		return -EFAULT;
	printk(KERN_INFO "buffer = %s\n", mem);
	return count;
}

ssize_t my_read ( struct file * filp , char __user *buf , size_t
	count , loff_t * offp ) 
{
	unsigned long i;
	char mem[100] = "ahoj";
	i = (info == 0) ? count : info;	
	if (i > 100)
		return -EFAULT;
	if (copy_to_user(buf, mem, i))
		return -EFAULT;
	return i;
}

int my_release ( struct inode *inode , struct file * filp ) {
	printk(KERN_INFO "modul uvolnen\n");
	return 0;
}

long my_unlocked_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{
	long ret = 0;
	unsigned int i;
	if (cmd == 1)
	{	
		info = (int)arg;
		printk(KERN_INFO "info in cmd=1 = %u\n", info);	
		return ret;		
	}
	if (cmd == 2)
	{
		i = info;
		put_user(i, (int*)arg); 
		printk(KERN_INFO "info in cmd=2 = %u\n", info);	
		return ret;
	}	
	return ret;
}

struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = my_open ,
	.write = my_write,
	.read = my_read,
	.release = my_release,
	.unlocked_ioctl =  my_unlocked_ioctl ,
};

struct miscdevice my_misc = {
	.minor = MISC_DYNAMIC_MINOR, .name = "my", .fops = &my_fops
};

static int my_init(void)
{
	misc_register(&my_misc);
	return 0;
}

static void my_exit(void)
{
	misc_deregister(&my_misc);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
