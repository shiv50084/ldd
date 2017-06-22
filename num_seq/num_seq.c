#include <linux/init.h>
#include <linux/module.h>

MODULE_AUTHOR("Vladimir Petrigo");
MODULE_DESCRIPTION("Print number sequence to /proc/num_seq");
MODULE_LICENSE("Dual BSD/GPL");

static int __init num_seq_init(void)
{
	return 0;
}

static void __exit num_seq_exit(void)
{
}

module_init(num_seq_init);
module_exit(num_seq_exit);
