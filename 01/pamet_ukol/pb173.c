#include <linux/module.h>
#include <linux/slab.h>

static int my_init(void)
{
	printk(KERN_INFO "Hello world\n");
	return 0;
}

static void my_exit(void)
{
	char* p = (kmalloc(1000, GFP_KERNEL);
	strcpy(p, "Bye");
	printk(KERN_INFO "%s\n", p);
	kfree(p);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");