#include <linux/module.h>
#include <linux/slab.h>
#include <linux/device.h>

static int my_init(void)
{
	printk(KERN_INFO "Hello\n");
	return 0;
}

static void my_exit(void)
{
	//allocation 1000B
	void *buf = kmalloc(1000, GFP_KERNEL);
	int i[3];
	//test of prope allocation and statements
	if (buf)
	{	
		strcpy((char*)buf, "Bye");
		printk(KERN_INFO "%s - value of alocated mamory\n", (char*)buf);
		printk(KERN_INFO "%p - kmalloc pointer address\n", buf);
	}
	//statements
	printk(KERN_INFO "%p - stack variable address\n", i);	
	printk(KERN_INFO "%p - jiffies\n", &jiffies);	
	printk(KERN_INFO "%p - funcion adress: printk\n", printk);
	printk(KERN_INFO "%p - funcion adress: bus_register\n", bus_register);
	printk(KERN_INFO "%pF - __builtin_return_address\n", __builtin_return_address(0));
	kfree(buf);			
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
