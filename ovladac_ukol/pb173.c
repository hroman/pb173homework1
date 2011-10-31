#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/device.h>

int info = 0; 		//characters to write
int write = 0;		//is any proccess writting?
DEFINE_MUTEX(my_mut);	//mutex

int my_open ( struct inode *inode , struct file * filp ) {
	int i;
	i = (filp->f_mode & FMODE_WRITE);
	if ( i != 0)
	{
		mutex_lock(&my_mut);
		if (write == 0)			
			write++;		
		else 
		{
			mutex_unlock(&my_mut);		
			return -EFAULT; 
		}	
	}
	mutex_unlock(&my_mut);
	printk(KERN_INFO "modul otevren\n");		
	return 0;
}

ssize_t my_write ( struct file * filp , const char __user *buf , size_t
	count , loff_t * offp ) 
{	
	char mem[100];
	if (count > 100)
		count = 100;
	mutex_lock(&my_mut);
	if (copy_from_user(mem, buf, count))
	{
		mutex_unlock(&my_mut);
		return -EFAULT;
	}
	mutex_unlock(&my_mut);
	printk(KERN_INFO "buffer = %s\n", mem);
	return count;
}

ssize_t my_read ( struct file * filp , char __user *buf , size_t
	count , loff_t * offp ) 
{
	unsigned long i;
	char mem[100] = "ahoj";
	mutex_lock(&my_mut);
	i = (info == 0) ? count : info;	
	if (i > 100)
	{
		mutex_unlock(&my_mut);	
		return -EFAULT;
	}
	if (copy_to_user(buf, mem, i))
	{
		mutex_unlock(&my_mut);	
		return -EFAULT;
	}
	mutex_unlock(&my_mut);	
	return i;
}

int my_release ( struct inode *inode , struct file * filp ) {
	//check if process is writer
	if (write != 0 && ( (filp->f_mode & FMODE_WRITE) != 0))
	{
		mutex_lock(&my_mut);
		write = 0;
		mutex_unlock(&my_mut);
	} 
	printk(KERN_INFO "modul uvolnen\n");
	return 0;
}

long my_unlocked_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{
	long ret = 0;
	unsigned int i;
	if (cmd == 1)
	{	
		mutex_lock(&my_mut);
		info = (int)arg;
		printk(KERN_INFO "info in cmd=1 = %u\n", info);	
		mutex_unlock(&my_mut);
		return ret;		
	}
	if (cmd == 2)
	{
		mutex_lock(&my_mut);
		i = info;
		ret = put_user(i, (int*)arg); 
		mutex_unlock(&my_mut);
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
