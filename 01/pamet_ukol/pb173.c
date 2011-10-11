#include <linux/module.h>
#include <linux/slab.h>

static int my_init(void)
{
	printk(KERN_INFO "homework1 - memory\n");
	return 0;
}

static void my_exit(void)
{
	void *buf = kmalloc(1000, GFP_KERNEL);
	if (buf)
	{
		strcpy((char*)buf, "Bye");
		printk(KERN_INFO "%s\n", buf);
	}	
	kfree(buf);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
