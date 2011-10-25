#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>

char mem[128];
DEFINE_MUTEX(my_mut1);
DEFINE_MUTEX(my_mut2);

/******************************************************************************
				MISC 1
*******************************************************************************/
int my_open1 ( struct inode *inode , struct file * filp ) {
	printk(KERN_INFO "modul1 otevren\n");		
	return 0;
}

ssize_t my_write1 ( struct file * filp , const char __user *buf , size_t
	count , loff_t * offp ) 
{
	int i;
	// count check
	if(count > 5)
		count = 5;
	//mutex lock
	mutex_lock(&my_mut1);
	for(i=0; i<count; i++)
	{
		//offp chech - offp position in buffer to write
		if (*offp >= strlen(mem))
			break;
		if (get_user(mem[*offp], &buf[i]))
		{
			mutex_unlock(&my_mut1);
			return -EFAULT;
		}		
		msleep(10);
		*offp += 1;	
	}
	mutex_unlock(&my_mut1);
	return i;
}

int my_release1 ( struct inode *inode , struct file * filp ) {
	printk(KERN_INFO "modul2 uzavren\n");
	return 0;
}

struct file_operations my_fops1 = {
	.owner = THIS_MODULE,
	.open = my_open1 ,
	.write = my_write1,
	.release = my_release1 ,
};

struct miscdevice my_misc1 = {
	.minor = MISC_DYNAMIC_MINOR, .name = "my_misc1", .fops = &my_fops1
};


/******************************************************************************
				MISC 2
*******************************************************************************/
int my_open2 ( struct inode *inode , struct file * filp ) {
	printk(KERN_INFO "modul2 otevren\n");		
	return 0;
}

ssize_t my_read2 ( struct file * filp , const char __user *buf , size_t			
	count , loff_t * offp )
{	
	//check count
	if (count > 128)
		count = 128;
	
	mutex_lock(&my_mut2);
	if (copy_to_user((void*)buf, mem, count))
	{
		mutex_unlock(&my_mut2);
		return -EFAULT;
	}
	mutex_unlock(&my_mut2);
	return count;
}

int my_release2 ( struct inode *inode , struct file * filp ) {
	printk(KERN_INFO "modul2 uzavren\n");
	return 0;
}

struct file_operations my_fops2 = {
	.owner = THIS_MODULE,
	.open = my_open2 ,
	.read = my_read2,
	.release = my_release2 ,
};

struct miscdevice my_misc2 = {
	.minor = MISC_DYNAMIC_MINOR, .name = "my_misc2", .fops = &my_fops2
};

static int my_init(void)
{
	misc_register(&my_misc2);
	misc_register(&my_misc1);
	return 0;
}

static void my_exit(void)
{
	misc_deregister(&my_misc1);
	misc_deregister(&my_misc2);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
