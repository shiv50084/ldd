#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Vladimir Petrigo");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple char device driver");

static int simple_major_no = 0;
static int simple_minor_no = 0;
static int simple_num_dev = 1;
static char *greet_msg = "Hello";
module_param(greet_msg, charp, S_IRUGO);

static struct cdev *my_cdev = NULL;

int simple_open(struct inode *, struct file *);
int simple_release(struct inode *, struct file *);
ssize_t simple_read(struct file *, char __user *, size_t, loff_t *);

static struct file_operations simple_ops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.release = simple_release,
    .read = simple_read
};

int simple_open(struct inode *inode, struct file *filp)
{	
	pr_debug("simple: open simple file\n");
	return 0;
}

int simple_release(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t simple_read(struct file *filp, char __user *buf, size_t count, loff_t *off)
{
    static char module_buf[] = "simple-read";
    int retval = 0;

    if (filp->f_pos + count > sizeof(module_buf)) {
        count = sizeof(module_buf);
    }
    
    pr_debug("simple: read count %zu\n", count);

    if (copy_to_user(buf, module_buf, count)) {
        retval = -EFAULT;
        goto out;
    }

    retval = count;

out:
    pr_debug("simple: read retval %d\n", retval);

    return retval;
}

static int __init simple_init(void)
{
	dev_t dev_no;
	int res = alloc_chrdev_region(&dev_no, simple_minor_no, 
			simple_num_dev, "simple");

	pr_debug("simple: %s\n", greet_msg);

	if (res < 0) {
		pr_warn("simple: can not get major number %d\n", simple_major_no);

		return res;
	}

	my_cdev = cdev_alloc();

	if (my_cdev == NULL) {
		return -EPERM;
	}	

	my_cdev->owner = THIS_MODULE;
	my_cdev->ops = &simple_ops;
	res = cdev_add(my_cdev, dev_no, 1);
	simple_major_no = MAJOR(dev_no);
	pr_debug("simple: allocate major number %d\n", simple_major_no);

	return 0;
}

static void __exit simple_exit(void)
{
	dev_t dev_no = MKDEV(simple_major_no, simple_minor_no);
	
	unregister_chrdev_region(dev_no, simple_num_dev);

	pr_debug("simple: Bye\n");
}

module_init(simple_init);
module_exit(simple_exit);
